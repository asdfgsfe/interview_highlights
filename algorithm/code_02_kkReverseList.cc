#include <assert.h>
#include <iostream>

struct ListNode
{
	int val;
	ListNode* next;
	ListNode() = default;
	ListNode(int v) : val(v), next(nullptr) {}
};

struct ReturnData
{
	ListNode* head;
	ListNode* tail;
	ReturnData() = default;
	ReturnData(ListNode* h, ListNode* t) : head(h), tail(t) {}
};

void Dump(const ListNode* head)
{
	while (head)
	{
		std::cout << head->val << " ";
		head = head->next;
	}
	std::cout << std::endl;
}

ReturnData ReverseKLenList(ListNode* head, int k)
{
	if (!head || !head->next || k < 1)
	{
		return ReturnData(head, nullptr);
	}
	int cnt = 0;
	ListNode* node = head;
	while (node && cnt != k)
	{ 
		++cnt;
		node = node->next;
	}
	if (cnt != k)
	{
		return ReturnData(head, nullptr);
	}
	ListNode* tail = nullptr;
	ListNode* pre = nullptr;
	while (head && k > 0)
	{
		tail = head->next;
		head->next = pre;
		pre = head;
		head = tail;
		--k;
	}
	return ReturnData(pre, tail);
}

ListNode* KKReverseList(ListNode* head, int k)
{
	if (!head || !head->next || k < 1)
	{
		return head;
	}
	ReturnData data = ReverseKLenList(head, k);
	if (data.head == head)
	{
		return head;
	}
	ListNode* newHead = data.head;
	ListNode* next = data.tail;
	ListNode* tail = head;
	while (next)
	{
		data = ReverseKLenList(next, k);
		tail->next = data.head;
		tail = next;
		next = data.tail;
	}
	return newHead;
}

int main(void)
{
	ListNode* head = new ListNode(1);
	head->next = new ListNode(2);
	head->next->next = new ListNode(3);
	head->next->next->next = new ListNode(4);
	head->next->next->next->next = new ListNode(5);
	head->next->next->next->next->next = new ListNode(6);
	head->next->next->next->next->next->next = new ListNode(7);
	Dump(head);
	head = KKReverseList(head, 3);
	Dump(head);

	return 0;
}