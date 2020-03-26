#include <vector>
#include <atomic>

enum Op { PUSH, POP, HEAD };

typedef struct Desc {
    const int value;
    const Op op;
    std::atomic_bool active = { true };

    Desc(int value, Op op);

} Desc;

typedef struct Node {
    const int value;
    const Op op;
    std::vector<Node*> nexts;
    std::atomic<Node*> prev = { nullptr };
    std::atomic<Desc*> desc = { nullptr };

    Node(int value, Op op);

} Node;

class QQueue {
    private:
        std::vector<Node*> tail;
        std::atomic<Node*> forkRequest;

        Node *head;

        bool insert(Node *cur, Node *elem, int index);
        bool remove(Node *cur, int index);
        int getRandomIndex();

    public:
        QQueue();
        ~QQueue();
        void push(int v);
        void pop(int &v);
};