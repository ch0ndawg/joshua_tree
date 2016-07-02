//
//  joshua_tree.cpp
//  
//
//  Created by Chris on 12/19/15.
//
// Structure named for Josh Tobin's suggestion of using a linked list with extra nodes

#include <cmath>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

struct jt_node {
    int val;
    jt_node *next;
    jt_node *super_next;
    jt_node(int v=0) : val(v), next(nullptr), super_next(nullptr) {}
};

class joshua_tree {
    jt_node *head;
    jt_node *tail;
    int sz;
    int sqrt_sz;
    
    jt_node node_group_at(int);
public:
    joshua_tree() : head(new jt_node), sz(0), sqrt_sz(0) {}
    joshua_tree(istream &is, int N);
    jt_node* node_at(int);
    int &operator [] (int i) {return node_at(i)->val;}

    void print(ostream &) const;
    int size() const {return sz ; }
    void query_1(int i, int j);
    void query_2(int i, int j);
    
    ~joshua_tree() { jt_node* curr =head;  while(curr) { jt_node* oldNext = curr->next; delete curr; curr=oldNext;} delete tail;}
};

inline ostream &operator<<(ostream&os, const joshua_tree& jt)
{
    jt.print(os);
    return os;
}

jt_node joshua_tree::node_group_at(int i)
{
    jt_node result;
    jt_node *curr = head->next, *prev = head;
    for (result.val=sqrt_sz; result.val<= i; result.val+= sqrt_sz) {
        prev = curr;
        curr = curr->super_next; // skip
    }
    result.next = curr; // hack
    result.super_next = prev;
    result.val -= sqrt_sz;
    return result;
}

jt_node *joshua_tree::node_at(int i)
{
    if (i < 0) return head;
    jt_node result = node_group_at(i);
    jt_node *curr = result.next;
    
    for (int k = result.val; k < i; k++) {
        curr = curr->next; // direct next
    }
    return curr;
}


void joshua_tree::query_1(int i, int j)
{
    if (i==0) return; // do nothing on move initial to initial
    
    int m = j-i+1; // number of elements to move
    
    jt_node res = node_group_at(i);
    int k=res.val;
    jt_node *oldStartTrailer = res.super_next; // previous start group
    bool firstGroup = (oldStartTrailer==head);
    jt_node *curr=res.next;
    
    jt_node *beforeStart = (i % sqrt_sz ? curr : node_at(i-1));
    
    
    
    for (; k < i; k++) {
        // prev=curr;
        beforeStart = curr;
        oldStartTrailer = oldStartTrailer->next;
        curr = curr->next; // direct next
    }
    
    //assert (oldStartTrailer->super_next == curr);
    jt_node *start = curr; // first node of the range to be moved
    
    res = node_group_at(j);
    k= res.val;
    jt_node *endTrailer = res.super_next; // previous end group
    jt_node *end = res.next; // end of the range
    jt_node *beforeEnd = head; // previous end
    
    for (; k < j; k++) {
        // prev=curr;
        beforeEnd = end;
        endTrailer = endTrailer->next;
        end = end->next; // direct next
    }

    jt_node* firstPast = end->next;
    
    beforeStart->next = firstPast; // collapse all the nodes between sp and end
    
    
    // DONE omitting the range
    
    // save old first node
    jt_node *oldHead = head->next;
    // set head to point to the moved start node
    head->next = head->super_next = start;
    // set old first node to follow end
    end->next = oldHead;
    
    jt_node *newSuper = firstPast;
    
    // now must clean up the "super" links
    
    // if it is the first group, adjust oldStartTrailer for the next set of statements
    if (firstGroup) {
        oldStartTrailer = oldHead;
        for (int ell = i; ell < sqrt_sz; ell++) // advance it until equal
                if(newSuper) newSuper = newSuper->next; // account for possible null nodes
    }
    
    // oldStartTrailer is previous start group
    // Goal here is to fix all the super next's of the sqrt_sz nodes that formerly pointed
    // *into* the range. Now they have to point to the new nodes *past* the range.
    while (oldStartTrailer != firstPast) { // loop until oldStartTrailer reaches one past the range; from there, the super next's are correct
        oldStartTrailer->super_next = newSuper;
        oldStartTrailer = oldStartTrailer->next;
        if (newSuper) newSuper = newSuper->next; // null out any of them at the end
    }
    
    // now fix the nodes in the end sqrt_sz elements of the moved nodes
    // they formerly pointed to the nodes just outside the range.
    // now they should be fixed to point to the former initial segment
    // (because this replaces the initial segment!)
    endTrailer = endTrailer->next;
    newSuper = oldHead;
    
    if (m<= sqrt_sz) { // if it is smaller than the maximum number that must be changed
        endTrailer = start;
        for (int ell=m; ell <sqrt_sz; ell++) {
            if (newSuper) newSuper = newSuper->next;
        }
    }
    
    while  (endTrailer != oldHead) {
        // keep moving endTrailer until we hit the end of the range, that is,
        // the old head of the list
        endTrailer->super_next = newSuper;
        endTrailer = endTrailer->next;
        if (newSuper) newSuper = newSuper->next;
    }
    
    if (j == sz-1) { // if range included end node, make it the new tail
        tail->next = beforeStart;
    }
}


void joshua_tree::query_2(int i, int j)
{
    if (j==sz-1) return; // do nothing on move final to final
    
    int m = j-i+1; // number of elements to move
    
    jt_node res = node_group_at(i);
   
    jt_node *oldStartTrailer = res.super_next; // previous start group
    bool firstGroup = (oldStartTrailer==head);
    bool lastGroup = (j >= sz - sqrt_sz);
    jt_node *curr=res.next;
    
    jt_node *beforeStart = (i % sqrt_sz ? curr : node_at(i-1));
    
    for (int k = res.val; k < i; k++) {
        // prev=curr;
        beforeStart = curr;
        oldStartTrailer = oldStartTrailer->next;
        curr = curr->next; // direct next
    }
    
    //assert (oldStartTrailer->super_next == curr);
    jt_node *start = curr; // first node to be moved
    
    res = node_group_at(j);
    jt_node *endTrailer = res.super_next; // previous end group
    jt_node *end=res.next; // end of the range
    jt_node *beforeEnd = (j % sqrt_sz ? nullptr : node_at(j-1));;
    
    for (int k = res.val; k < j; k++) {
        // prev=curr;
        beforeEnd = end;
        endTrailer = endTrailer->next;
        end = end->next; // direct next
    }
    
    
    
    jt_node* firstPast = end->next;
    //node_group_at(res, sz-1); // get last sqrt(N) elements
    jt_node *oldLastTrailer = node_at(sz-sqrt_sz);
    
    
    beforeStart->next = firstPast; // collapse all the nodes between sp and end
    
    jt_node *oldTail = tail->next;
    end->next = nullptr;
    tail->next = end; // set the end of the rage to the end of the whole list
    oldTail->next = start;
    
    if (i==0) { // first group was moved; reset head
        head->next = head->super_next = firstPast;
    }
    // crazy ass idea
    
    //query_1(i,j); // first move the range to the front as before...
    //query_1(m,sz-1); // now move the entire complement to the beginning!!
    
    
    jt_node *newSuper = firstPast;
    
    // if it is the first group, adjust oldStartTrailer for the next set of statements
    if (firstGroup) {
        oldStartTrailer = head->next;
        for (int ell = i; ell < sqrt_sz; ell++) // advance it until equal
            if(newSuper) newSuper = newSuper->next; // account for possible null nodes
    }
    
    // oldStartTrailer is previous start group
    // Goal here is to fix all the super next's of the sqrt_sz nodes that formerly pointed
    // *into* the range. Now they have to point to the new nodes *past* the range.
    while (oldStartTrailer != firstPast) { // loop until oldStartTrailer reaches one past the range; from there, the super next's are correct
        oldStartTrailer->super_next = newSuper;
        oldStartTrailer = oldStartTrailer->next;
        if (newSuper) newSuper = newSuper->next; // null out any of them at the end
    }
    
    // now fix the nodes in the end sqrt_sz elements of the moved nodes
    // they formerly pointed to the nodes just outside the range.
    // now they should be nulled out, because they're the final entries in the list
    endTrailer = endTrailer->next;
    
    if (m<= sqrt_sz)  // if it is smaller than the maximum number that must be changed
        endTrailer = start;
    
    while (endTrailer) {
        // keep moving endTrailer until we hit the end of the range, that is,
        // the old head of the list
        endTrailer->super_next = nullptr;
        endTrailer = endTrailer->next;
    }
    
    curr = start;
    if (lastGroup) {
        while (oldLastTrailer) {
            if (curr) curr = curr->next;
            oldLastTrailer = oldLastTrailer->next;
        }
        oldLastTrailer = firstPast;
    }
    
    while (oldLastTrailer != start) {
        oldLastTrailer->super_next = curr;
        if (!curr) break;
        oldLastTrailer = oldLastTrailer->next;
        curr = curr->next;
    }
}


joshua_tree::joshua_tree(istream &is, int N)
: sz(N), sqrt_sz(static_cast<int>(sqrt(N))), head(new jt_node), tail(new jt_node)
{
    int x;
    jt_node *curr = head;
    int k;
    for (k=0; k<sqrt_sz;k++) {
        is >> x;
        curr->next = new jt_node(x);
        curr = curr->next;
    }
    
    jt_node *super_curr = head->next;
    head->super_next = head->next;
    for (k= sqrt_sz; k < sz; k++) {
        is >> x;
        curr->next = new jt_node(x);
        curr = curr->next;
        super_curr->super_next = curr; // have the super next's point to the beginning of the next block
        super_curr = super_curr->next;
    }
    
    tail->next = curr;
    tail->super_next = super_curr; // WARNING: this points oppositely!!
}


void joshua_tree::print(ostream &os) const
{
    jt_node *curr = head->next;
    for (int i=0; i<sz; i++) {
#ifdef DEBUG
        os << curr->val << '(' << curr << ", " << curr->next << ", " << curr->super_next << ")\n" ;
#else
        os << curr->val << ' ';
#endif
        curr = curr->next;
    }
}

/* // #define NO_GENERATE 0
int main() {
    // brute force:

    int N=0, M=0;
    cin >> N >> M;
#ifdef NO_GENERATE
   joshua_tree jt(cin,N);
    
    cout << jt << endl;
    for (int k=0; k<M; k++) {
        int choice, i,j;
        cin >> choice >> i >> j;
        if (choice == 1) {
            jt.query_1(i-1,j-1); // subtract 1 for the zero-based array indexing
        } else {
            jt.query_2(i-1,j-1);
        }    // cout << abs(jt[N-1]-jt[0]) << endl;

    }
    cout << abs(jt[N-1]-jt[0]) << endl;
    cout << jt << endl;
#else
    stringstream s;
    for (int i=1; i<=N; i++) {
        s << i << ' ';
    }
    joshua_tree jt(s,N);
    cout << jt << endl;
    for (int i = 2; i<=N-1; i++) {
        for (int j = i; j <= N-1; j++){
            cout << "Query 2: " << i << ' ' << j << endl;
            jt.query_2(i-1,j-1);
            cout << jt << endl;
        }
    }
#endif
    return 0;
}
*/