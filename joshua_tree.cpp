//
//  joshua_tree.cpp
//  
//
//  Created by Chris on 12/19/15.
//
//

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
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
public:
    joshua_tree() : head(new jt_node), sz(0), sqrt_sz(0) {}
    joshua_tree(istream &is, int N);
    jt_node* node_at(int);
    int node_group_at(jt_node &, int);
    int &operator [] (int i) {return node_at(i)->val;}
  //  void insert(int data);
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

//void joshua_tree::insert(int data)
//{
//    jt_node oldTail = tail->next;
//    tail->next = new jt_node(data);
//    oldTail->next = tail->next;
//    
//    if ( sqrt(++sz) >= sqrt_sz+1) {
//        sqrt_sz++;
//        
//        jt_node *curr = head->next;
//        jt_node *prev = head->next;
//        
//        
//        while (curr) {
//            prev = curr;
//            curr = curr->super_next;
//            prev->super_next = curr->next; // update the super next's to be all
//        }
//        curr = prev;
//        while (curr) {
//            prev = curr;
//            curr = curr->next;
//        }
//    }
//}

int joshua_tree::node_group_at(jt_node& result, int i)
{
    jt_node *curr = head->next, *prev = head;
    int k;
    for (k=sqrt_sz; k<= i; k+= sqrt_sz) {
        prev = curr;
        curr = curr->super_next; // skip
    }
    result.next = curr; // hack
    result.super_next = prev;
    return k-sqrt_sz;
}

jt_node *joshua_tree::node_at(int i)
{
    jt_node result;
    if (i < 0) return head;
    int k = node_group_at(result,i);
    jt_node *curr = result.next;
    
    for (; k < i; k++) {
       // prev=curr;
        curr = curr->next; // direct next
    }
    return curr;
}


void joshua_tree::query_1(int i, int j)
{
    if (i==0) return; // do nothing on move initial to initial
    
    int m = j-i+1; // number of elements to move
    
    jt_node res;
    int k= node_group_at(res,i);
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
    
    k = node_group_at(res,j);
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
    
    jt_node res;
    int k= node_group_at(res,i);
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
    jt_node *start = curr; // first node to be moved
    
    k = node_group_at(res,j);
    jt_node *endTrailer = res.super_next; // previous end group
    jt_node *end=res.next; // end of the range
    jt_node *beforeEnd = head;
    
    for (; k < j; k++) {
        // prev=curr;
        beforeEnd = end;
        endTrailer = endTrailer->next;
        end = end->next; // direct next
    }
    
    node_group_at(res, sz-1); // get last sqrt(N) elements
    jt_node *plg= res.next;
    
    jt_node* firstPast = end->next;
    
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
    while (plg != start) {
        plg->super_next = curr;
        if (!curr) break;
        plg = plg->next;
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
        os << curr->val << ' ' ; //<< '(' << curr << ", " << curr->next << ", " << curr->super_next << ")\n" ;
        curr = curr->next;
    }
}

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    // brute force:

    int N=0, M=0;
    cin >> N >> M;
    //M=3;
    
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
        
        cout << "After Query " << choice << ' ' << i << ' ' << j << ':' << endl;
        cout << jt << endl;

    }
    // cout << abs(jt[N-1]-jt[0]) << endl;
    
  //  cout << "After Query:" << endl;
  //  cout << jt << "\n\nBy index:\n";
    
   // cout << jt[7]->va
    
    cout << jt << "\n\nBy index:\n";
   /* for (int i=0; i< N; i++) {
        cout << jt[i] << ' ' <<endl ;
    }
    cout << endl;
    
    //cout << "JT went byebye";
    */
    return 0;
}