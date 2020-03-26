#include "../headers/qqueue.hpp"

#define FAIL_THRESHOLD 10

Desc::Desc( int value, Op op ): value( value ), op( op ) {}
Node::Node( int value, Op op ): value( value ), op( op ) {}

QQueue::QQueue() {
    head = new Node( 0, HEAD );
    tail.push_back(head);
}

void QQueue::push( int v ) {
    Node *elem = new Node( v, PUSH );
    bool ret = false;
    int loops = 0;

    while ( true ) {
        loops++;

        int index = getRandomIndex();
        Node *cur = tail[index];

        if ( cur == nullptr ) {
            continue;
        }

        if ( cur->op == POP ) {
            ret = remove( cur, v );
        } else {
            ret = insert( cur, elem, v );
        }

        if ( ret ) {
            break;
        }

        if ( loops > FAIL_THRESHOLD && !forkRequest.load() ) {
            Node *null = nullptr;
            forkRequest.compare_exchange_strong( null, cur );
            break;
        }
    }
}

void QQueue::pop(int &v) {
    Node *elem = new Node(v, POP);
    bool ret = false;
    int loops = 0;

    while ( true ) {
        loops++;

        int index = getRandomIndex();
        Node *cur = tail[index];
        
        if ( cur == nullptr ) {
            continue;
        }

        if ( cur->op == PUSH ) {
            ret = remove( cur, v );
        } else {
            ret = insert( cur, elem, v );
        }

        if ( ret ) {
            break;
        }

        if ( loops > FAIL_THRESHOLD && !forkRequest.load() ) {
            Node *null = nullptr;
            forkRequest.compare_exchange_strong( null, cur );
            break;
        }
    }
}

bool QQueue::insert(Node *cur, Node *elem, int index) {
    Desc *d = new Desc( elem->value, elem->op );
    Desc *curDesc = cur->desc;

    if ( curDesc->active ) {
        return false;
    }

    if ( cur->desc.compare_exchange_strong( curDesc, d ) ) {
        /*if ( top[index] ) {
            
        }
        ???
        */

        if ( cur->nexts.empty() /* && top.count(cur) == 1 */ ) {
            elem->prev.store(cur);
            cur->nexts.push_back(elem);
            tail[index] = elem;
            Node *helperNode = forkRequest.load();
            if ( helperNode != nullptr && forkRequest.compare_exchange_strong( helperNode, nullptr ) ) {
                if ( helperNode->op == cur->op ) {
                    helperNode->prev.store(cur);
                    cur->nexts.push_back(helperNode);
                    // helperNode
                }
            }

            d->active = false;
            return true;
        } else {
            //
        }
    }

    d->active = false;
    return false;
}

bool QQueue::remove(Node *cur, int index) {
    Desc *d = new Desc(-1, cur->op);
    Desc *curDesc = cur->desc;
    Node *prev = cur->prev;

    if ( curDesc->active ) {
        return false;
    }

    if ( cur->desc.compare_exchange_strong( curDesc, d ) ) {
        /*
            if ( top[index] ) {

            }
        */

       if ( cur->nexts.empty() ) {
           // v = cur.value
           //prev->nexts.remove(cur);
           tail[index] = prev;
           d->active = false;
           return true;
       } else {
           //remove dead branch
       }
    }

    d->active = false;
    return false;
}

int QQueue::getRandomIndex() {

}