#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Python.h"

int fac(int n)
{
	if (n < 2) return(1);
	return n*fac(n-1);
}

char* reverse(char *s)
{
	char t;
	char *p = s;
	char *q = s + strlen(s) - 1;
	while (p < q)
	{
		t = *p;
		*p++ = *q;
		*q-- = t;
	}
	return s;
}

static PyObject* Extest_fac(PyObject* self, PyObject *args)
{
	int num;
	if(!PyArg_ParseTuple(args, "i", &num))
	{
		return NULL;
	}
	return (PyObject*)Py_BuildValue("i", fac(num));
}

static PyObject* Extest_doppel(PyObject* self, PyObject *args)
{
	char* origin;
	char* dup;
	PyObject* retval;

	if(!PyArg_ParseTuple(args, "s", &origin))
	{
		return NULL;
	}
	dup = strdup(origin);
	retval = (PyObject*)Py_BuildValue("ss", origin, reverse(dup));
	free(dup);
	return retval;
}

static PyMethodDef ExtestMethods[] = 
{
	{"fac", Extest_fac, METH_VARARGS},
	{"doppel", Extest_doppel, METH_VARARGS},
	{NULL, NULL}
};

void initExtest()
{
	Py_InitModule("Extest", ExtestMethods);
}


int main()
{
	char s[BUFSIZ];
	printf(" 4! == %d \n", fac(4));
	printf(" 8! == %d \n", fac(8));
	printf(" 12! == %d \n", fac(12));
	strcpy(s, "abcdef");
	printf("reversing 'abcdef', we get '%s'\n", reverse(s));
	strcpy(s, "student");
	printf("reversing 'student', we get '%s'\n", reverse(s));
	return 0;
}
