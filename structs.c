#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "externs.h"
#include "utils.h"
#include "structs.h"


/* This function adds a extern node to the extern linked list */
ext_ptr add_ext(ext_ptr *hptr, char *name, unsigned int reference)
{
    ext_ptr t=*hptr;
    ext_ptr temp;

    temp=(ext_ptr) malloc(sizeof(ext)); /*allocate memory for the node*/
    if(!temp)
    {
        fprintf(stderr, "Error: couldn't allocate memory for extern node\n");
        exit(ERROR);
    }

    temp -> address = reference;
    strcpy(temp->name, name);

    if(!(*hptr)) /* checks if the linked list is empty */
    {
        *hptr = temp;
        temp -> next = temp;
        temp -> prev = temp;
        return temp;
    }

    ((*hptr)->prev)->next = temp;
    temp->next = t;
    temp->prev = t->prev;
    (*hptr)->prev = temp;

    return temp;
}

/* This function frees the memory for the extern linked list, by free each node */
void free_ext(ext_ptr *hptr)
{
    ext_ptr temp=*hptr;
    if(temp) {
        unsigned int last_reference = ((*hptr)->prev)->address;
        unsigned int reference = 0;
        do {
            temp = *hptr;
            reference = temp->address;
            *hptr = (*hptr)->next;
            free(temp);
        } while (reference != last_reference);
    }
}

/* This function prints the extern linked list */
void print_ext(ext_ptr h)
{
    ext_ptr orig=h;
    do
    {
        printf("\nname: %s, reference: %d - >", h->name, h->address);
        h=h->next;
    } while(h -> address != orig -> address);
    printf("*\n");
}

/*This function offsets the labels addresses by given numer*/
void offset_label_address(label_ptr label, int num, bool is_data)
{
    while(label)
    {   /*as long as it's not an external label and the values of is_data and is_instruction_sentence is different*/
        if(!(label -> external) && (is_data != (label -> is_instruction_sentence))) 
        {
            label -> address += num; /*offsets the address by the given number*/
        }
        label = label -> next;
    }
}


/*This function changes given label entry field to TRUE and returns TRUE or returns FALSE if label isn't exist */
int change_entry_in_label(label_ptr h, char *name)
{
    label_ptr label = get_label(h, name); /*get label pointer from list by label name*/
    if(label != NULL)
    {
        if(label -> external)
        {
            error_mode = ENTRY_CANNOT_APPLY_TO_EXTERN;
            return FALSE;
        }
        label -> entry = TRUE;
        entry_exists = TRUE;
        return TRUE;
    }
    else
        error_mode = ENTRY_IS_NOT_FOLLOWED_BY_LABEL;
    return FALSE;
}

/* This function returns the given label address*/
unsigned int get_label_address(label_ptr h, char *name)
{
    label_ptr label = get_label(h, name);
    if(label != NULL)
        return label -> address;
    return FALSE; /*if label doesn't exist*/
}

/*This function checks if given label is an existing extern label*/
bool is_external_label(label_ptr h, char *name)
{
    label_ptr label = get_label(h, name);
    if(label != NULL)
        return label -> external;
    return FALSE;
}

/*This function checks if given label exist in the list*/
bool is_label_exist(label_ptr h, char *name)
{
    return get_label(h, name) != NULL;
}

/*This function returns pointer to the given label if exist*/
label_ptr get_label(label_ptr h, char *name)
{
	while(h)
	{
        if(strcmp(h->name,name)==0) 
			return h;
		h=h->next;
	}
	return NULL; /*No label with that name found in the list*/
}

/*This function adds given label to the labels linked list */
label_ptr add_label(label_ptr *hptr, char *name, unsigned int address, bool external, ...)
{	
	va_list p;
	label_ptr t=*hptr;
	label_ptr temp;

	if(is_label_exist(*hptr, name)) /*checks if there is a label with that name*/
	{
		error_mode = LABEL_NAME_ALREADY_EXISTS;
		return NULL;
	}
	temp=(label_ptr) malloc(sizeof(Labels)); /*allocate memory for the temporary variable*/
	if(!temp) 
	{
		fprintf(stderr, "Error: couldn't allocate memory for label node\n");
		exit(ERROR);
	}

	strcpy(temp->name, name); /*copy the label name*/
    temp -> entry = FALSE; /*sets label entry field to FALSE*/
	temp -> address = address; /*sets label address*/
	temp -> external = external; /*sets label external field*/

	if(!external) /*checks if label isn't external*/
	{
		va_start(p,external);
		temp -> is_instruction_sentence = va_arg(p,bool);
	}
    else
    {
        extern_exists = TRUE;
    }

	if(!(*hptr)) /*if linked list is empty we set the temp to the head of the list*/
	{	
		*hptr = temp;
		temp -> next = NULL;
		return temp;	
	}

	while(t -> next != NULL) /*pointing to the last label in the list*/
		t = t->next;
	temp -> next = NULL;
	t -> next = temp;

	va_end(p);
	return temp;
}

/* This function frees the memory of the symbols table, by free each node*/
void free_labels(label_ptr *hptr)
{
	label_ptr temp;
	while(*hptr)
	{
		temp=*hptr;
		*hptr=(*hptr)->next;
		free(temp);
	}
}


/*This function deletes given label from the linked list,
 and return if we deleted it successesfully*/
int delete_label(label_ptr *hptr, char *name)
{
    label_ptr temp = *hptr;
    label_ptr prevtemp;
    while (temp) 
    {
        if (strcmp(temp->name, name) == 0) /*if label name exist*/
        {
            if (strcmp(temp->name, (*hptr)->name) == 0)
            {
                *hptr = (*hptr)->next;
                free(temp); /*deletes label by freeing the memory*/
            } 
            else 
            {
                prevtemp->next = temp->next;
                free(temp); /*deletes label by freeing the memory*/
            }
            return TRUE;
        }
        prevtemp = temp;
        temp = temp->next; /*change the previous label pointer to the next*/
    }
    return FALSE;

}

/* This function prints the labels linked list */
void print_labels(label_ptr h)
{
    while (h)
    {
        printf("\nname: %s, address: %d, external: %d", h->name, h->address, h->external);
        if (h->external == 0)
            printf(", is in action statement: %d -> ", h->is_instruction_sentence);
        else
            printf(" -> ");
        h = h->next;
    }
    printf("*");
}