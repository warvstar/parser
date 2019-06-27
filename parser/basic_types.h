#pragma once
typedef struct _string {
	char* data;
	unsigned long long size;
	unsigned long long capacity;
}string;
typedef struct _any {
	unsigned long long pointer;
	string type;
}any;