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

    /**
     * Follow similar style to original impl. in paper.
     * @param v
     */
    public void queue(int v) {
        Node elem = new Node(v, Queue);
        boolean ret = false;
        int loops = 0;
        while (true) {
            loops++;

            // Since we have to Queue / Deque from two different places,
            // we have for each node.
            int index = getRandomTailIndex();

            // Instead of creating desc inside the remove/insert function,
            // lets do it here.
            Desc d = new Desc(v, Queue);
            elem.desc.set(d);

            // We try catch here, in the case that the index we are choosing is out of bounds.
            // Note that we still check if the index we chose, corresponds to the node we are
            // expecting, so this works.
            try {
                Node cur = tail.get(index);
                
                // Since we have unfinished deque, let's get unfinished deque in order.
                if (cur.op == Deque) {
                    index = getRandomHeadIndex();
                    cur   = head.nexts.get(index);

                    // We still have to make sure we choose a node with deque in it.
                    if (cur.op == Deque) {
                        ret   = remove(cur, index, d);
                    }

                    // Normal queuing operation.
                } else {
                    ret = insert(cur, elem, index, d);
                }

                // This is called if either random indeces are out of bounds.
                // It's never the case that if we grab an index, we will have the wrong Node
                // for that index during insert / remove.
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

    /**
     * Follows same style as queue, except we reverse the operations.
     * @param adr
     */
    public void dequeue(MutableInt adr) {
        Node elem = new Node(adr, Deque);
        boolean ret = false;
        int loops = 0;

        // Much of the same, just reversed.
        while (true) {
            loops++;
            int index = getRandomHeadIndex();

            Desc d = new Desc(adr, Deque);
            elem.desc.set(d);

            try {
                Node cur = head.nexts.get(index);
                // if we have a prev. succesful queue, remove it and return that result.
                if (cur.op == Queue) {
                    ret = remove(cur, index, d);
                } else {
                    // We have to grab from other side of tree, and add a failed op tail node.
                    index = getRandomTailIndex();
                    cur   = tail.get(index);
                    ret = insert(cur, elem, index, d);
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                // Instead of just continuing, we may be out of index
                // if nexts is empty. In that case, we actually got an invalid
                // operation, so we insert.
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

    /**
     * Same style as the orig impl. except we do not use top, and we use try catches
     * to handle out of bounds indeces, since we actively delete nodes from the list.
     */
    public boolean insert(Node cur, Node elem, int index, Desc d) {
        Desc curDesc = cur.desc.get();

        if (curDesc.active.get()) {
            return false;
        }

        // Once we get this, we are in control of this node exclusively, no other
        // thread can operate on this node.
        if(cur.desc.compareAndSet(curDesc, d)) {

            // We could throw here, if any index was deleted mid-operation.
            // A deletion operation may also shift elements, which we check against here.
            // Lastly, we check that tail is still what we expect, in case it has already been queued on.
            try {
                if (tail.get(index) != cur) {
                    d.active.set(false);
                    return false;
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                d.active.set(false);
                return false;
            }

            // We only queue to empty leaf nodes.
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

                        // This is important, if we dont release active, then no one can act on this node.
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

    /**
     * Same idea as insert, except since we operate on head, we can delete on non-leaf nodes.
     * Furthermore, we have to update head and tail pointers simulatenously.
     * This is where we shift all the elements for tail / head.
     */
    public boolean remove(Node cur, int index, Desc d) {
        Desc curDesc = cur.desc.get();

        if (curDesc.active.get()) {
            return false;
        }

        // Try to take control as usual.
        if (cur.desc.compareAndSet(curDesc, d)) {
            // Make sure we are looking at the correct node.
            try {
                if (head.nexts.get(index) != cur) {
                    d.active.set(false);
                    return false;
                }
            } catch (ArrayIndexOutOfBoundsException e) {
                d.active.set(false);
                return false;
            }

            // We dont want to return true on empty, since most threads use remove
            // as a way to complete a pop.
            if (!head.nexts.isEmpty()) {

                // Now, we have to add all the of deque'd node's children to head's children.
                // We must also update their prev.
                for (Node node : cur.nexts) {
                    node.prev.set(head);
                    head.nexts.add(node);
                }

                // Remove dequeu'd elem fron head.
                head.nexts.remove(cur);

                // Try to remove elem from tail as well, if it exist.
                // Since tail is thread safe container, this is safe.
                int tIndex = tail.indexOf(cur);
                if (tIndex != -1) {
                    tail.set(tIndex, head);
                }

                // Obscure impl details, basically just handles which desc contains the adr.
                if (cur.op == Queue) {
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