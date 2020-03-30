package qqueue;

import java.util.Vector;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

import static qqueue.Op.*;

class Node {

    private static final int INVALID = -1;

    final int value;
    final Op op;
    final Vector<Node> nexts;
    final AtomicReference<Node> prev;
    final AtomicReference<Desc> desc;

    public Node(Op op) {
        this.value = INVALID;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(null, op));
    }

    public Node(int value, Op op) {
        this.value = value;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(null, op));
    }

    public Node(MutableInt adr, Op op) {
        this.value = INVALID;
        this.op = op;
        this.nexts = new Vector<>(100);
        this.prev = new AtomicReference<>(null);
        this.desc = new AtomicReference<>(new Desc(adr, op));
    }
}

class Desc {
    final MutableInt adr;
    final Op op;
    final AtomicBoolean active;

    public Desc(MutableInt adr, Op op) {
        this.adr = adr;
        this.op = op;
        active = new AtomicBoolean(true);
    }

    public Desc(Op op) {
        this(null, op);
    }
}

class QQueue {
    private final Node head;
    private final Vector<Node> tail;

    public QQueue() {
        this.head = new Node(HEAD);
        this.head.desc.get().active.set(false);

        tail = new Vector<>(100);
        tail.add(head);
    }

    public void queue(int v) {
        Node elem = new Node(v, PUSH);
        boolean ret = false;
        int loops = 0;
        while (true) {
            loops++;
            int index = getRandomIndex();
            Node cur = tail.get(index);
            if (cur == null) {
                continue;
            }

            ret = insert(cur, elem, index);

            if (ret) {
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
            int index = getRandomIndex();
            Node cur = tail.get(index);

            ret = remove(cur, index, elem.desc.get());

            if (ret) {
                break;
            }
        }
    }

    public boolean insert(Node cur, Node elem, int index) {
        Desc d = elem.desc.get();
        Desc curDesc = cur.desc.get();

        if (curDesc.active.get()) {
            return false;
        }

        if (cur.nexts.isEmpty()) {
            elem.prev.set(cur);
            cur.nexts.add(elem);
            tail.set(index, elem);

            d.active.set(false);
            return true;
        }

        return false;
    }

    public boolean remove(Node cur, int index, Desc d) {
        Desc curDesc = cur.desc.get();
        Node prev = cur.prev.get();

        if (curDesc.active.get()) {
            return false;
        }

        // Take control of node cur.
        // a -> {b} -> c -> d
        if (cur.desc.compareAndSet(curDesc, d)) {
            while(true) {
                // Take control of prev.
                // {a} -> {b} -> c -> d
                if (prev.desc.get().active.compareAndSet(false, true)) {
                    //       {b} -> c -> d
                    // {a} <
                    //       c -> d
                    prev.nexts.addAll(cur.nexts);
                    // {a} -> c -> d
                    prev.nexts.remove(index);

                    prev.desc.get().active.set(false);
                }

                // Since no other thread can enter a, or b, they cant update its lists.

                int tIndex = tail.indexOf(cur);
                if (tIndex != -1) {
                    tail.set(tIndex, prev);
                }

                d.active.set(false);

                d.adr.value = cur.value;

                return true;
            }
        }

        return false;
    }

    public int getRandomIndex() {
        return 0;
    }
}