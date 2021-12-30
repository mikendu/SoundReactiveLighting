#ifndef LINKED_LIST_H
#define LINKED_LIST_H



template <typename T> struct BufferNode
{
    T* item = nullptr;
    BufferNode<T>* prev = nullptr;
    BufferNode<T>* next = nullptr;

    BufferNode() {}
    BufferNode(T* data) : item(data) {}
};


template <typename T> class LinkedList
{    
public:

    LinkedList(): count(0) {}
    
    void add(T* item) 
    {
        add(new BufferNode<T>(item));
    }

    void add(BufferNode<T>* node) 
    {
        if (count == 0) 
        {
            head = node;
            tail = node;
            node->next = nullptr;
            node->prev = nullptr;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
            node->next = nullptr;
            tail = node;
        }
        count += 1;
    }
    
    void remove(BufferNode<T>* node) 
    {
        // Should check for count == 0
        // Should verify that the node given is head/tail
        if (count == 1) 
        {
            head = nullptr;
            tail = nullptr;
        }
        else
        {
            // should be the case if not head
            if (node->prev != nullptr) 
            {
                node->prev->next = node->next;
            }
            else
            {
                head = node->next;
            }

            // should be the case if not tail
            if (node->next != nullptr)
            {
                node->next->prev = node->prev;
            }
            else 
            {
                tail = node->prev;
            }
        }

        node->prev = nullptr;
        node->next = nullptr;
        count -= 1;
    }


    uint8_t size()
    {
        return count;
    }

    BufferNode<T>* first()
    {
        return head;
    }

    BufferNode<T>* last()
    {
        return tail;
    }

private:

    BufferNode<T>* head;
    BufferNode<T>* tail;
    uint8_t count;


};


#endif