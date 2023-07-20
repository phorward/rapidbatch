/*
J.M.K S.F. Dynamic Variable System v3.4 (DVS3)
Copyright (C) 2000-2005 by J.M.K S.F., Jan Max Meyer
*/

#pragma hdrstop

#define VC_COMPILE

#include "rb5_varsys.h"

#pragma package(smart_init)
// #############################################################################
// newVar
// #############################################################################
int newVar(LIST* list, char* vname, void* data, int area, int* array_info)
{
	static int dim = 0;
	NODE* new_ptr = (NODE*)malloc(sizeof(NODE));
	memset(new_ptr, NULL, sizeof(NODE));

	if (list->ptr_begin == NULL)
	{
		list->ptr_begin = new_ptr;
		list->ptr_end = new_ptr;
		new_ptr->prev = NULL;
	}
	else
	{
		list->ptr_end->next = new_ptr;
		new_ptr->prev = list->ptr_end;
		list->ptr_end = new_ptr;
	}

	if (list->varType == VAR_STRING)
	{
		new_ptr->varValue = strdup((char*)data);
		//(char*) malloc((strlen((char*)data) + 1) * sizeof(char));
		//strcpy((char*)new_ptr->varValue, (char*)data);
	}

	new_ptr->varName = strdup(vname);
	//(char*) malloc(strlen(vname)+1);
	//strcpy((char*)new_ptr->varName, vname);
	new_ptr->ptr_pointer = (NODE*) NULL;

	if (dim == 0)
		new_ptr->varArea = area;


	if (dim < array_info[0])
	{
		new_ptr->child_list = (LIST*) malloc(sizeof(LIST));
		memset((LIST*)new_ptr->child_list, NULL, sizeof(LIST));
		LIST* temp = (LIST*)new_ptr->child_list;
		temp->varType = list->varType;

		dim++;
		for (int a = 0; a < array_info[dim];a++)
		{
			newVar((LIST*)new_ptr->child_list, "", "", -1, array_info);
		}
		dim--;
	}

	return VARSYS_OKAY;
}

// #############################################################################
// letVar
// #############################################################################
int letVar(LIST* list, char* vname, void* data, int area, int* array_info)
{
	static int dim = 0;
	int ret = VARSYS_OKAY;

	NODE* pcurrent;

	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0
			        && (pcurrent->varArea == area || pcurrent->varArea == 0)) break;
			pcurrent = pcurrent->prev;
		}

		if (pcurrent == (NODE*) NULL )
		{
			return VARSYS_ERR_VARNOTFOUND;
		}

		while (pcurrent->ptr_pointer != (NODE*) NULL)
		{
			pcurrent = pcurrent->ptr_pointer;
		}
	}
	else
	{
		pcurrent = list->ptr_begin;

		int c = 0;
		while (pcurrent != (NODE*) NULL)
		{
			if (c == array_info[dim]) break;
			pcurrent = pcurrent->next;
			c++;
		}
		if (pcurrent == (NODE*) NULL)
			return VARSYS_ERR_INDEXOUTOFRANGE;
	}

	if (dim < array_info[0])
	{
		if (pcurrent->child_list == (LIST*) NULL)
			return VARSYS_ERR_INDEXOUTOFRANGE;

		dim++;
		ret = letVar((LIST*)pcurrent->child_list, "", data, -1, array_info);
		dim--;
	}
	else
	{
		free(pcurrent->varValue);

		if (list->varType == VAR_STRING)
		{
			pcurrent->varValue = strdup((char*)data);
			// (char*) malloc(strlen((char*)data) + 1);
			//strcpy((char*)pcurrent->varValue, (char*)data);
		}
	}

	return ret;
}

// #############################################################################
// getVar
// #############################################################################
int getVar(LIST* list, char* vname, int area, int* array_info, char** retval)
{
	static int dim = 0;
	int ret = VARSYS_OKAY;
	NODE* pcurrent;

	if (dim == 0)
	{
		if (*retval != (char*)NULL) free(*retval);
		*retval = (char*)NULL;

		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0
			        && (pcurrent->varArea == area || pcurrent->varArea == 0)) break;
			pcurrent = pcurrent->prev;
		}

		if (pcurrent == (NODE*) NULL)
		{
			return VARSYS_ERR_VARNOTFOUND;
		}

		while (pcurrent->ptr_pointer != (NODE*) NULL)
		{
			pcurrent = pcurrent->ptr_pointer;
		}
	}
	else
	{
		pcurrent = list->ptr_begin;
		int c = 0;
		while (c < array_info[dim] && pcurrent != (NODE*) NULL)
		{
			pcurrent = pcurrent->next;
			c++;
		}
	}


	if (pcurrent == NULL)
	{
		return VARSYS_ERR_VARNOTFOUND;
	}

	if (dim < array_info[0])
	{
		if (pcurrent->child_list == (LIST*) NULL)
		{
			return false;
		}

		dim++;
		ret = getVar((LIST*)pcurrent->child_list, NULL, -1, array_info, retval);
		dim--;
	}
	else
	{
		(char*)*retval = strdup((char*)pcurrent->varValue);
		/*(char*)malloc((strlen((char*)(pcurrent->varValue))+1) * sizeof(char));
		strcpy(*retval, (char*)pcurrent->varValue);*/
	}

	return ret;
}

// #############################################################################
// resizeVar
// #############################################################################
int resizeVar(LIST* list, char* vname, void* data, int area, int* array_info)
{
	static int dim = 0;

	NODE* pcurrent;
	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0
			        && (pcurrent->varArea == area || pcurrent->varArea == 0)) break;
			pcurrent = pcurrent->prev;
		}


		if (pcurrent == (NODE*) NULL)
		{
			return VARSYS_ERR_VARNOTFOUND;
		}

		while (pcurrent->ptr_pointer != (NODE*) NULL)
		{
			pcurrent = pcurrent->ptr_pointer;
		}

		if (array_info[0] > 0)
		{
			if (pcurrent->child_list == (LIST*) NULL)
			{
				pcurrent->child_list = (LIST*) malloc(sizeof(LIST));
				memset((LIST*)pcurrent->child_list, NULL, sizeof(LIST));
				LIST* temp = (LIST*)pcurrent->child_list;
				temp->varType = list->varType;
			}

			dim++;
			resizeVar((LIST*) pcurrent->child_list, "", "", -1, array_info);
			dim--;
		}
	}
	else
	{
		pcurrent = list->ptr_begin;

		NODE* pprev = (NODE*) NULL;
		int c = 0;
		while (c <= array_info[dim])
		{
			//Warning: Check pcurrent Nodes to NULL ALWAYS as the first condition,
			//         because if pcurrent is NULL, pcurrent->next can't be accessed and
			//         we'll get a program crash!!!!!
			if (pcurrent == (NODE*) NULL)
			{

				/*char hello[100];
				sprintf(hello, "In Dimension %d lege Node %d", dim, c);
				MessageBox(NULL, hello, "debug", 0);
				*/


				NODE* new_ptr = (NODE*)malloc(sizeof(NODE));
				memset(new_ptr, NULL, sizeof(NODE));

				if (list->ptr_begin == (NODE*) NULL) list->ptr_begin = new_ptr;
				list->ptr_end = new_ptr;

				new_ptr->prev = pprev;
				new_ptr->next = (NODE*) NULL;

				if (pprev != (NODE*) NULL)
					pprev->next = new_ptr;

				if (list->varType == VAR_STRING)
				{
					new_ptr->varValue = (char*) malloc(1);
					strcpy((char*)new_ptr->varValue, "");
					//strcpy((char*)new_ptr->varValue, "Test");
				}
				new_ptr->ptr_pointer = (NODE*) NULL;
				pcurrent = new_ptr;
			}

			if (c == array_info[dim] && dim < array_info[0])
			{
				if (pcurrent->child_list == (LIST*) NULL)
				{
					pcurrent->child_list = (LIST*) malloc(sizeof(LIST));
					memset((LIST*)pcurrent->child_list, NULL, sizeof(LIST));
					LIST* temp = (LIST*)pcurrent->child_list;
					temp->varType = list->varType;
				}
				/*
				char hello[100];
				sprintf(hello, "In Dimension %d lege List an fuer %d", dim, c);
				MessageBox(NULL, hello, "debug", 0);
				*/

				dim++;
				resizeVar((LIST*) pcurrent->child_list, "", "", -1, array_info);
				dim--;
			}

			pprev = pcurrent;
			pcurrent = pcurrent->next;

			c++;
		}
	}

	return VARSYS_OKAY;
}

/*
// #############################################################################
// cpyNodes (Hilfsfunktion fuer cpyVar)
// #############################################################################
int cpyNodes(LIST* dest, LIST* src)
{
        NODE* dest_node = dest->ptr_begin;
        NODE* src_node = src->ptr_begin;

        while(dest_node != (NODE*) NULL &&
                src_node != (NODE*) NULL)
        {
                if(dest->varType == VAR_STRING)
                {
                        strcpy((char*) dest_node->varValue, (char*) src_node->varValue);
                }
                if(dest_node->child_list != (LIST*) NULL &&
                        src_node->child_list != (LIST*) NULL)
                {
                        cpyNodes((LIST*)dest_node->child_list, (LIST*) src_node->child_list);
                }

                dest_node = dest_node->next;
                src_node = src_node->next;
        }
        return 0;
}

// #############################################################################
// cpyVar
// #############################################################################
int cpyVar(LIST* list, char* source, int* array_info, char* target)
{
        static int dim = 0;

	NODE* pcurrent;
        NODE* pdestcurrent;

        if(dim == 0)
        {
                pcurrent = list->ptr_end;
        	while (pcurrent != NULL)
        	{
                        if(strcmp(pcurrent->varName, source) == 0) break;
        		pcurrent = pcurrent->prev;
        	}
        }
        else
        {
                pcurrent = list->ptr_begin;

                int c = 0;
                while(pcurrent != NULL)
                {
                        if(c == array_info[dim]) break;
                        pcurrent = pcurrent->next;
                        c++;
                }

        }

        if(pcurrent == NULL) return -1;

        if(dim < array_info[0])
        {
                dim++;
                cpyVar((LIST*)pcurrent->child_list, "", array_info, target);
                dim--;
        }
        else
        {
                pdestcurrent = list->ptr_end;
        	while (pdestcurrent != (NODE*)NULL)
        	{
                        if(strcmp(pdestcurrent->varName, target) == 0) break;
        		pdestcurrent = pdestcurrent->prev;
        	}

                if(list->varType == VAR_STRING)
                {
                        strcpy((char*) pdestcurrent->varValue, (char*) pcurrent->varValue);
                }

                cpyNodes((LIST*)pdestcurrent->child_list, (LIST*)pcurrent->child_list);

                if(pdestcurrent != (NODE*) NULL)
                {
                        pdestcurrent->child_list = pcurrent->child_list;
                }
        }

        return 0;
}
*/

// #############################################################################
// ptrVar
// #############################################################################
int ptrVar(LIST* list, char* source, int src_area, int* array_info, LIST* tar_list, char* target, int tar_area )
{
	static int dim = 0;
	int ret = VARSYS_OKAY;

	NODE* pcurrent;
	NODE* pdestcurrent;

	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, source) == 0 &&
			        (pcurrent->varArea == src_area || pcurrent->varArea == 0)) break;
			pcurrent = pcurrent->prev;
		}

		if (pcurrent != NULL)
		{
			while (pcurrent->ptr_pointer != (NODE*) NULL)
			{
				pcurrent = pcurrent->ptr_pointer;
			}
		}
	}
	else
	{
		if ( list == (LIST*)NULL)
			return VARSYS_ERR_INDEXOUTOFRANGE;

		pcurrent = list->ptr_begin;

		int c = 0;
		while (pcurrent != NULL)
		{
			if (c == array_info[dim]) break;
			pcurrent = pcurrent->next;
			c++;
		}

		if (pcurrent == (NODE*) NULL)
			return VARSYS_ERR_INDEXOUTOFRANGE;
	}

	if (pcurrent == NULL)
		return VARSYS_ERR_VARNOTFOUND;

	if (dim < array_info[0])
	{
		dim++;
		ret = ptrVar((LIST*)pcurrent->child_list, "", src_area, array_info, tar_list, target, tar_area);
		dim--;
	}
	else
	{
		pdestcurrent = tar_list->ptr_end;
		while (pdestcurrent != NULL)
		{
			if (strcmp(pdestcurrent->varName, target) == 0 &&
			        pdestcurrent->varArea == tar_area) break;

			pdestcurrent = pdestcurrent->prev;
		}

		//if(pdestcurrent != (NODE*) NULL) pdestcurrent->child_list = pcurrent->child_list;
		if (pdestcurrent != (NODE*) NULL)
			pdestcurrent->ptr_pointer = pcurrent;

		//memcpy(pdestcurrent, pcurrent, sizeof(NODE*));
	}
	return ret;
}


// #############################################################################
// delVar
// #############################################################################
int delVar(LIST* list, char* vname, int* array_info, bool complete)
{
	static int dim = 0;
	int area = -1;
	NODE* pcurrent;
	NODE* pdel;

	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0) break;
			pcurrent = pcurrent->prev;
		}
		if (pcurrent == (NODE*)NULL)
			return VARSYS_ERR_VARNOTFOUND;


		area = pcurrent->varArea;

		if (!complete)
		{
			while (pcurrent->ptr_pointer != (NODE*) NULL)
			{
				pcurrent = pcurrent->ptr_pointer;
			}
		}

		if (pcurrent->child_list != (LIST*)NULL)
		{
			dim++;
			delVar((LIST*)pcurrent->child_list, NULL, array_info, true);
			if (array_info[0] == 0)
			{
				free(pcurrent->child_list);
				pcurrent->child_list = (LIST*)NULL;
			}
			dim--;
		}

		if (complete && pcurrent != (NODE*) NULL)
		{
			if (pcurrent->prev != (NODE*) NULL)
			{
				pcurrent->prev->next = pcurrent->next;
				if (list->ptr_end == pcurrent)
				{
					list->ptr_end = pcurrent->prev;
				}
			}
			if (pcurrent->next != (NODE*) NULL)
			{
				pcurrent->next->prev = pcurrent->prev;
				if (list->ptr_begin == pcurrent)
				{
					list->ptr_begin = pcurrent->next;
				}
			}

			if (pcurrent->varName != (char*)NULL)
				free(pcurrent->varName);
			if (pcurrent->varValue != (void*)NULL)
				free(pcurrent->varValue);

			free(pcurrent);
		}
		else
		{
			letVar(list, (char*)vname, (char*)"", area, array_info);
		}
	}
	else
	{
		if (dim <= array_info[0])
		{
			pcurrent = list->ptr_begin;
			int c = 0;
			while (c < array_info[dim] && pcurrent != (NODE*) NULL)
			{
				pcurrent = pcurrent->next;
				c++;
			}

			if (pcurrent->child_list != (LIST*) NULL)
			{
				dim++;
				delVar((LIST*)pcurrent->child_list, NULL, array_info, true);
				if (dim > array_info[0])
				{
					free(pcurrent->child_list);
					pcurrent->child_list = (LIST*)NULL;
				}
				dim--;
			}
		}
		else
		{
			pcurrent = list->ptr_begin;
			while (pcurrent != (NODE*)NULL)
			{
				if (pcurrent->child_list != (LIST*) NULL)
				{
					dim++;
					delVar((LIST*)pcurrent->child_list, NULL, array_info, true);
					free(pcurrent->child_list);
					pcurrent->child_list = (LIST*)NULL;
					dim--;
				}
				pdel = pcurrent;
				pcurrent = pcurrent->next;

				if (pdel != (NODE*)NULL)
				{
					if (pdel->varName != (char*)NULL)
						free(pdel->varName);
					if (pdel->varValue != (void*)NULL)
						free(pdel->varValue);

					free(pdel);
				}
			}
		}
	}
	return VARSYS_OKAY;
}


// #############################################################################
// varSize
// #############################################################################
long varSize(LIST* list, char* vname, int area, int* array_info)
{
	static int dim = 0;
	long elem = 0;
	NODE* pcurrent;

	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0
			        && (pcurrent->varArea == area || pcurrent->varArea == 0)) break;
			pcurrent = pcurrent->prev;
		}

		if (pcurrent == (NODE*) NULL)
		{
			return -1;
		}

		while (pcurrent->ptr_pointer != (NODE*) NULL)
		{
			pcurrent = pcurrent->ptr_pointer;
		}
	}
	else
	{
		pcurrent = list->ptr_begin;
		if (array_info[dim] > -1)
		{
			int c = 0;
			while (pcurrent != NULL)
			{
				if (c == array_info[dim]) break;
				pcurrent = pcurrent->next;
				c++;
			}
		}
	}

	if (pcurrent == (NODE*)NULL)
	{
		return -1;
	}

	if (dim < array_info[0])
	{
		if (pcurrent->child_list == (LIST*) NULL)
		{
			return -1;
		}
		dim++;
		elem = varSize((LIST*)pcurrent->child_list, NULL, -1, array_info);
		dim--;
	}
	else
	{
		pcurrent = list->ptr_begin;
		while (pcurrent != (NODE*) NULL)
		{
			elem++;
			pcurrent = pcurrent->next;
		}
	}

	return elem;
}

// #############################################################################
// varExists
// #############################################################################
bool varExists(LIST* list, char* vname, int area, int* array_info)
{
	static int dim = 0;
	bool tmp = false;
	NODE* pcurrent;

	if (dim == 0)
	{
		pcurrent = list->ptr_end;
		while (pcurrent != (NODE*)NULL)
		{
			if (strcmp(pcurrent->varName, vname) == 0)
			{
				if (pcurrent->varArea != area && pcurrent->varArea != 0)
				{
					return false;
				}

				if (array_info[0] > 0 && pcurrent->child_list != (LIST*)NULL)
				{
					dim++;
					tmp = varExists((LIST*)pcurrent->child_list, "", -1, array_info);
					dim--;
					return tmp;
				}
				return true;
			}
			pcurrent = pcurrent->prev;
		}
	}
	else
	{
		pcurrent = list->ptr_begin;
		int c = 0;
		while (c <= array_info[dim])
		{
			if (pcurrent == (NODE*) NULL)
			{
				return false;
			}
			else if (c == array_info[dim])
			{
				if (dim < array_info[0])
				{
					if (pcurrent->child_list != (LIST*)NULL)
					{
						dim++;
						tmp = varExists((LIST*)pcurrent->child_list, "", -1, array_info);
						dim--;
						return tmp;
					}
					return false;
				}
				else
				{
					return true;
				}
			}
			pcurrent = pcurrent->next;
			c++;
		}

	}
	return false;
}


// #############################################################################
// shutdownVarSys
// #############################################################################
void shutdownVarSys(LIST* list)
{
	NODE* pcurrent;
	NODE* pprev;

	pcurrent = list->ptr_end;
	while (pcurrent != (NODE*)NULL)
	{
		if (pcurrent->child_list != (void*)NULL)
		{
			shutdownVarSys((LIST*)pcurrent->child_list);
			free(pcurrent->child_list);
		}
		if (pcurrent->varValue != (void*)NULL)
			free(pcurrent->varValue);
		if (pcurrent->varName != (char*)NULL)
			free(pcurrent->varName);

		pprev = pcurrent->prev;
		free(pcurrent);
		pcurrent = pprev;
	}
}

// #############################################################################
// printVar
// #############################################################################
void printVar(LIST* list, char* file)
{
	NODE* pcurrent;
	static int dim = 0;
	static FILE* debug_out = (FILE*)NULL;

	if (file != (char*)NULL)
	{
		debug_out = fopen(file, "at");
		fprintf(debug_out, "------------\n");
	}

	pcurrent = list->ptr_begin;
	int cnt = 0;
	while (pcurrent != (NODE*)NULL)
	{
		for (int a = 0; a < dim; a++)
		{
			fprintf(debug_out, "\t");
		}

		if (dim == 0)
		{
			fprintf(debug_out, "%p %s: >%s<\n", pcurrent, (char*) pcurrent->varName, (char*) pcurrent->varValue);
		}
		else
		{
			fprintf(debug_out, "%p %d: >%s<\n", pcurrent, cnt, (char*) pcurrent->varValue);
		}
		cnt++;

		if ((LIST*)pcurrent->child_list != (LIST*)NULL)
		{
			dim++;
			printVar((LIST*)pcurrent->child_list, (char*)NULL);
			dim--;
		}
		pcurrent = pcurrent->next;
	}

	if (file != (char*)NULL)
		fclose(debug_out);
}

