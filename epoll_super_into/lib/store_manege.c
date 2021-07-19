#include "StoreManage.h"

Node *Init()
{
	Node *p;
	p = (Node *)malloc(sizeof(Node));
	if (p == NULL)
	{
		perror("malloc");
		return 0;
	}

	strcpy(p->St_Info.Store_Name, "NULL");
	strcpy(p->St_Info.Introduce, "NULL");
	p->St_Info.Store_Num = -1;
	p->St_Info.Number = -1;
	p->St_Info.price = -1;
	

	p->pre = NULL;
	p->next = NULL;

	return p;
}

int Order(Node *head, char *buf, int flag)
{
	Node *p = head->next;

	while(p != NULL)
	{
		if (strcmp(p->St_Info.Store_Name, buf) == 0)
		{
			if (flag == 1)//下单
			{
				p->St_Info.Store_Num=p->St_Info.Store_Num-1;

				return 1;
			}
			else
			{
				p->St_Info.Store_Num=p->St_Info.Store_Num+1;
				return 1;
			}
		}

		p = p->next;
	}

	return 0;
}

void WriteFile(Node *head)
{
	Node *p = head->next;
	FILE *fp = fopen("商品信息.txt", "w");
	while(p != NULL)
	{
		fprintf(fp, "%s\t%d\t%d\t%d\t%s\n", p->St_Info.Store_Name, p->St_Info.Store_Num, p->St_Info.Number, p->St_Info.price, p->St_Info.Introduce);

		p = p->next;
	}
	fclose(fp);
}

int SendInfo(Node *head, int sockfd)
{
	Node p;
	FILE *fp = fopen("商品信息.txt", "r");
	int flag = -1;
	while(1)
	{
		fscanf(fp, "%s\t%d\t%d\t%d\t%s\n", p.St_Info.Store_Name, &(p.St_Info.Store_Num), &(p.St_Info.Number), &(p.St_Info.price), p.St_Info.Introduce);
		send(sockfd, &flag, sizeof(flag), 0);
		send(sockfd, &(p.St_Info), sizeof(p.St_Info), 0);
		if (feof(fp) != 0)
		{
			flag = 1;
			printf("传送完成\n");
			send(sockfd, &flag, sizeof(flag), 0);
			return 1;
		}
	}

	fclose(fp);
	return 1;
}

void ScanfLink(Node *head)
{
	FILE *fp = fopen("商品信息.txt", "r");
	Node *p, *q;

	while (feof(fp) == 0)
	{	
		p = Init();
		q = head;

		fscanf(fp, "%s\t%d\t%d\t%d\t%s\n", p->St_Info.Store_Name, &(p->St_Info.Store_Num), &(p->St_Info.Number), &(p->St_Info.price), p->St_Info.Introduce);

		while(q->next != NULL)
			q = q->next;
		p->next = head->next;
		head->next = p;

		p->pre = head;
	}
	fclose(fp);
}
/*
void InsertInfo(Node *head)
{
	Node *q = head;
	Node *p;
	p = (Node *)malloc(sizeof(Node));
	int i = 1;

	printf("商品名称:");
	scanf("%s", p->St_Info.Store_Name);
	p->St_Info.Store_Num = i++;
	printf("商品数量:");
	scanf("%d", &p->St_Info.Number);

	if (head->next == NULL)
	{
		p->next = head->next;
		head->next = p;

		p->pre = head;
	}
	else
	{
		p->next = head->next;
		head->next = p;

		head->next->pre = p;
		p->pre = head;
	}
}*/
