#include <iostream>                                                                                              

struct ListNode
{
	int val;
	ListNode* next;
	ListNode() = default;
	ListNode(int v) : val(v), next(nullptr) {}
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

ListNode* TwoTwoReverseList(ListNode* head)
{
	if (!head || !head->next)
	{
		return head;
	}
	ListNode* newHead = head->next;
	ListNode* cur = head;
	ListNode* future = head->next;
	while (cur && future)
	{
		ListNode* next = future->next;
		future->next = cur;
		cur->next = next ? (next->next ? next->next : next): nullptr;
		cur = next;
		future = next ? next->next : nullptr;
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
	//head->next->next->next->next->next = new ListNode(6);
	Dump(head);
	head = TwoTwoReverseList(head);
	Dump(head);

	return 0;
}