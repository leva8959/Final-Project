#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <stdexcept>
#include <string>

// --- 1. Data Type: Movie Structure ---
struct Movie {
    std::string title;
    int year;
    double rating; // filmscore (IMDb rating/score)

    // constructor
    Movie(const std::string& t, int y, double r)
        :title(t), year(y), rating(r) {}
    // default constructor
    Movie() : title(""), year(0), rating(0.0) {}

    // overload output operator
    friend std::ostream& operator<<(std::ostream& os, const Movie& m) {
        os << "\"" << m.title << "\" (" << m.year << "), Rating: " << m.rating;
        return os;
    }
};

// --- 2. The Link Structure: Node ---
template <typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;

    Node(const T& val) : data(val), next(nullptr), prev(nullptr) {}
};

// --- 3. The Underlying Structure: Doubly Linked List ---
template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t count;

    Node<T>* getNodeAt(int index) const {
        if (index < 0 || index >= count) {
            return nullptr;
        }
    Node<T>* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
        }
    return current;
    }
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // destructor (cleans up memory)
    ~DoublyLinkedList() {
       while (head != nullptr) {
            Node<T>* temp = head;
            head = head->next;
            delete temp;
        } 
        tail = nullptr;
    }
    
    const T& getElementAt(int index) const {
        Node<T>* node = getNodeAt(index);
        if (node == nullptr) {
            throw std::out_of_range("Index out of bounds for search.");
        }
        return node->data;
    }

    void add_back(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }
    void add_front(const T& val) {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            head->prev = newNode;
            newNode->next = head;
            head = newNode;
        }
        count++;
    }
    void remove_back() {
        if (is_empty()) {
            throw std::out_of_range("*Deque is empty. Can't remove_back.*");
        }
        Node<T>* oldTail = tail;
        if (head == tail) {
            head = nullptr;
            tail = nullptr;
        }
        else {
            tail = tail->prev;
            tail->next = nullptr;
        }
        delete oldTail;
        count--;
    }
    void remove_front() {
        if (is_empty()) {
            throw std::out_of_range("*Deque is empty. Can't remove_front.*");
        }
        Node<T>* oldHead = head;
        if (head == tail) {
            head = nullptr;
            tail = nullptr;
        }
        else {
            head = head->next;
            head->prev = nullptr;
        }
        delete oldHead;
        count--;
        }
        const T& get_front() const {
            if (is_empty()) {
                throw std::out_of_range("*Deque is empty. Can't get_front.*");
            }
            return head->data;
        }
        const T& get_back() const {
            if (is_empty()) {
                throw std::out_of_range("*Deque is empty. Can't get_back.*");
            }
            return tail->data;
        }
        bool is_empty() const {
            return head == nullptr;
        }
        size_t size() const {
            return count;
        }
};


    // --- 4. Interface - Deque ADT ---
    template <typename T>
    class Deque {
    private:
        DoublyLinkedList<T> list;

    public:
        const T& getElementAt(int index) const {
            return list.getElementAt(index);
        }
        // insert element BACK
        void push_back(const T& val) { list.add_back(val); }

        // insert element FRONT
        void push_front(const T& val) { list.add_front(val); }

        // remove LAST element
        void pop_back() { list.remove_back(); }

        // remove FIRST element
        void pop_front() { list.remove_front(); }

        // examine LAST element
        T& back() const { return list.get_back(); }

        // examine FIRST element
        T& front() const { return list.get_front(); }

        // utility
        bool is_empty() const { return list.is_empty(); }
        size_t size() const { return list.size(); }
};
#endif // PROJECT_H