package qqueue;

import java.util.Vector;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

import static qqueue.Op.*;

class Node {

    private static final int INVALID = -1;

    final MutableInt adr;
    final int value;
    final Op op;
    final Vector<Node> nexts;
    final AtomicReference<Node> prev;
    final AtomicReference<Desc> desc;

    public Node(Op op) {
        this.adr = null;
        this.value = INVALID;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(null, op));
    }

    public Node(int value, Op op) {
        this.adr = null;
        this.value = value;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(value, op));
    }

    public Node(MutableInt adr, Op op) {
        this.adr = adr;
        this.value = INVALID;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(adr, op));
    }

    public String toString() {
        return String.format("{Op:%s, Value:%d}", op, value);
    }
}

class Desc {

    private static final int INVALID = -22222;

    final int value;
    final MutableInt adr;
    final Op op;
    final AtomicBoolean active;

    public Desc(MutableInt adr, Op op) {
        this.value = INVALID;
        this.adr = adr;
        this.op = op;
        active = new AtomicBoolean(true);
    }

    public Desc(int value, Op op) {
        this.value = value;
        this.op = op;
        this.adr = null;
        active = new AtomicBoolean(true);
    }

    public Desc(Op op) {
        this(null, op);
    }
}

public class QQueue {

    private static final int FAIL_THRESHOLD = 5;

    private final Node head;
    private final Vector<Node> tail;
    private final AtomicReference<Node> forkRequest;

    public QQueue() {
        this.head = new Node(HEAD);
        this.head.desc.get().active.set(false);

        tail = new Vector<>(100);
        tail.add(head);

        forkRequest = new AtomicReference<>(null);
    }

    public void queue(int v) {
        Node elem = new Node(v, PUSH);
        boolean ret = false;
        int loops = 0;
        while (true) {
            loops++;
            int index = getRandomTailIndex();

            Desc d = new Desc(v, PUSH);
            elem.desc.set(d);

            try {
                Node cur = tail.get(index);
                if (cur.op == POP) {
                    index = getRandomHeadIndex();
                    cur   = head.nexts.get(index);
                    if (cur.op == POP) {
                        ret   = remove(cur, index, d);
                    }
                } else {
                    ret = insert(cur, elem, index, d);
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                continue;
            }

            if (ret) {
                break;
            }

            if (loops > FAIL_THRESHOLD && forkRequest.get() == null) {
                forkRequest.compareAndSet(null, elem);
                break;
            }
        }
    }

    public void dequeue(MutableInt adr) {
        Node elem = new Node(adr, POP);
        boolean ret = false;
        int loops = 0;
        while (true) {
            loops++;
            int index = getRandomHeadIndex();

            Desc d = new Desc(adr, POP);
            elem.desc.set(d);

            try {
                Node cur = head.nexts.get(index);
                if (cur.op == PUSH) {
                    ret = remove(cur, index, d);
                } else {
                    index = getRandomTailIndex();
                    cur   = tail.get(index);
                    ret = insert(cur, elem, index, d);
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                if (head.nexts.isEmpty()) {
                   ret = insert(head, elem,0, d);
                } else {
                    continue;
                }
            }

            if (ret) {
                break;
            }

            if (loops > FAIL_THRESHOLD && forkRequest.get() == null) {
                forkRequest.compareAndSet(null, elem);
                break;
            }
        }
    }

    public boolean insert(Node cur, Node elem, int index, Desc d) {
        Desc curDesc = cur.desc.get();

        if (curDesc.active.get()) {
            return false;
        }

        if(cur.desc.compareAndSet(curDesc, d)) {
            try {
                if (tail.get(index) != cur) {
                    d.active.set(false);
                    return false;
                }
            } catch (Exception e) {
                d.active.set(false);
                return false;
            }
            if (cur.nexts.isEmpty()) {
                elem.prev.set(cur);
                cur.nexts.add(elem);
                tail.set(index, elem);

                Node helperNode = forkRequest.get();
                if (helperNode != null && forkRequest.compareAndSet(helperNode, null)) {
                    if (helperNode.op == cur.op) {
                        helperNode.prev.set(cur);
                        cur.nexts.add(helperNode);
                        tail.add(helperNode);
                        helperNode.desc.get().active.set(false);
                    }
                }

                d.active.set(false);
                return true;
            }
        }

        d.active.set(false);
        return false;
    }

    public boolean remove(Node cur, int index, Desc d) {
        Desc curDesc = cur.desc.get();

        if (curDesc.active.get()) {
            return false;
        }

        if (cur.desc.compareAndSet(curDesc, d)) {
            try {
                if (head.nexts.get(index) != cur) {
                    d.active.set(false);
                    return false;
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                d.active.set(false);
                return false;
            }

            if (!head.nexts.isEmpty()) {
                for (Node node : cur.nexts) {
                    node.prev.set(head);
                    head.nexts.add(node);
                }

                head.nexts.remove(cur);

                int tIndex = tail.indexOf(cur);
                if (tIndex != -1) {
                    tail.set(tIndex, head);
                }

                if (cur.op == PUSH) {
                    d.adr.value.set(cur.value);
                } else {
                    cur.adr.value.set(d.value);
                }

                d.active.set(false);
                return true;
            }
        }

        d.active.set(false);
        return false;
    }

    public int getRandomHeadIndex() {
        return (int)(Math.random() * head.nexts.size());
    }

    public int getRandomTailIndex() {
        return (int)(Math.random() * tail.size());
    }
}