#include <iostream>
#include "my_heap.h"

using namespace std;

My_heap::My_heap()
{
	heap_begin = nullptr;
	blk = nullptr;
	used_bytes = 0;
}
/////////////////////////////////////////////////////////////
memory_block* My_heap::bump_allocate(int num_bytes)
{
	//If there is not enough memory in the heap to allocate num_bytes, the function should return nullptr
	if(used_bytes + num_bytes > MAX_CAPACITY) 
	{
		return nullptr;
	}

	memory_block* b = new memory_block(); //creating b1 

	//If it is the first one
	if(heap_begin == nullptr && blk == nullptr)
	{
		heap_begin = b;
		blk = b;

		b->left = nullptr;
		b->right = nullptr;

		b->used = true;
		b->starting_address = 0;
		b->size = num_bytes;
		used_bytes = num_bytes; //Img_heap' used_bytes must be increase	
	}

	//If it is not the first one, we have to add it at the end
	else
	{

		blk->right = b;
		b->left = blk;
		blk = b;
		
		b->right = nullptr;
		
		b->used = true;
		b->starting_address = used_bytes;
		b->size = num_bytes;
		used_bytes += num_bytes;
	}
	return b;
}
/////////////////////////////////////////////////////////////
void My_heap::deallocate(memory_block* to_delete)
{
	to_delete->used = false; // since it is free in all cases

	//First, it can be at the end:

	if(to_delete->left != nullptr && to_delete->right == nullptr)
	{
		if(to_delete->left->used == false)
		{
			to_delete->left->size += to_delete->size;
			blk = to_delete->left;
			to_delete->left->right = nullptr;
			to_delete = nullptr;
			delete to_delete;
		}
	}
	
	//Second, it can be at the beggining

	else if(to_delete->right != nullptr && to_delete->left == nullptr)
	{
		if(to_delete->right->used == false)
		{
			to_delete->right->starting_address = to_delete->starting_address;
			//if there is a ERROR about used_bytes look here.
			to_delete->right->size += to_delete->size;
			heap_begin = to_delete->right;
			to_delete->right->left = nullptr;
			to_delete = nullptr;
			delete to_delete;
		}
	}

	// Third, it can be in the middle

	else if(to_delete->left != nullptr && to_delete->right != nullptr)
	{
		//Three cases can occur:
	
		//case-1:
		if(to_delete->left->used == false && to_delete->right->used == true)
		{
			to_delete->left->size += to_delete->size;
			to_delete->left->right = to_delete->right;
			to_delete->right->left = to_delete->left;
	
			delete to_delete;
		}
		//case-2:
		else if(to_delete->right->used == false && to_delete->left->used == true)
		{
			to_delete->right->starting_address = to_delete->starting_address;
			to_delete->right->size += to_delete->size;
			to_delete->right->left = to_delete->left;
			to_delete->left->right = to_delete->right;
			to_delete = nullptr;
			delete to_delete;
		}
		//case-3:
		else if(to_delete->right->used == false && to_delete->left->used == false)
		{
			memory_block* to_delete2 = to_delete->right;
			to_delete2->starting_address = to_delete->starting_address;
			to_delete2->size += to_delete->size;
			to_delete2->left = to_delete->left;
			to_delete->left->right = to_delete2;
			to_delete = nullptr;
			delete to_delete;

			memory_block* to_delete3 = to_delete->left;
			to_delete2->starting_address = to_delete3->starting_address;
			to_delete2->size += to_delete3->size;
			to_delete3->left->right = to_delete2;
			to_delete2->left = to_delete3->left;
			to_delete3 = nullptr;
			delete to_delete3;
		}
		
	}
}
/////////////////////////////////////////////////////////////
memory_block * My_heap::first_fit_allocate(int num_bytes)
{
	memory_block* p = heap_begin;
	while(p != nullptr)
	{
		if(p->used == false && p->size >= num_bytes) //if I can find a block
		{
			p->used = true;
			return p;
		}
		p = p->right;
	}
	return bump_allocate(num_bytes); // bump_allocate will return a pointer so I again should return a new pointer
}
/////////////////////////////////////////////////////////////
memory_block * My_heap::best_fit_allocate(int num_bytes)
{
	memory_block* p = heap_begin;
	memory_block* smallest = nullptr;
	int difference = MAX_CAPACITY;

	while(p != nullptr)
	{
		if(p->used == false && p->size >= num_bytes) //if I can find a block to fit it
		{
			if(p->size - num_bytes <= difference)
			{
				difference = p->size - num_bytes;
				smallest = p;
			}
		}
		p = p->right;
	}

	if(smallest != nullptr) // which means that we find a smallest block 
	{
		smallest->used = true;
		return smallest;
	}
	else 
	{
		return bump_allocate(num_bytes); // else...
	}
}
/////////////////////////////////////////////////////////////
memory_block * My_heap::first_fit_split_allocate(int num_bytes)
{
	memory_block* po = heap_begin;
	
	while(po != nullptr)
	{
		if(po->used == false && po->size == num_bytes) //if I can find a block which is exactly the same size
		{
			po->used = true;
			return po;
		}
		else if(po->used == false && po->size > num_bytes) //or with a bigger size...
		{
			memory_block* po2 = new memory_block(); //for splitting size we create a new node

			po->right->left = po2;
			po2->right = po->right;
			po->right = po2;
			po2->left = po;

			po2->size = po->size - num_bytes;
			po->size = num_bytes;

			po->used = true;
			po2->used = false;

			po2->starting_address = po->starting_address + po->size;
			return po;
		}
		po = po->right;
	}
	return bump_allocate(num_bytes);
}
/////////////////////////////////////////////////////////////
float My_heap::get_fragmantation()
{
	float free_memory = MAX_CAPACITY;
	float biggest_free_block = 0;
	float remain_of_block = MAX_CAPACITY;
	float fragmentation = 0;

	memory_block* look = heap_begin;

	if(look == nullptr)
	{ 
		return fragmentation;
	}

	while(look != nullptr)
	{
		if(look->used == true)
		{
			free_memory -=  look->size;
			remain_of_block -=  look->size;
		}

		else
		{
			remain_of_block -=  look->size;
			if(look->size > biggest_free_block)
			{
				biggest_free_block = look->size;
			}
		}

		look = look->right;
	}
	if(biggest_free_block < remain_of_block)
	{
		biggest_free_block = remain_of_block;
	}

	fragmentation = (free_memory - biggest_free_block)/free_memory * 100;
	return fragmentation;
}
/////////////////////////////////////////////////////////////
void My_heap::print_heap()
{
	int total_mem_blocks = 0;
	int total_used_mem_blocks = 0;
	int total_free_mem_blocks = 0;
	int currently_used = 0;

	memory_block* look = heap_begin;

	if(look == nullptr)
	{
		cout << "Maximum capacity of heap: 512B" << endl;
		cout << "Currently used memory (B): "  << currently_used << endl;
		cout << "Total memory blocks: "  << total_mem_blocks << endl;
		cout << "Total used memory blocks: " << total_used_mem_blocks << endl;
		cout << "Total free memory blocks: " << total_free_mem_blocks << endl;
		cout << "Fragmentation: " << get_fragmantation() << "% " << endl;
		cout << "------------------------------" << endl;
	}
	else
	{
		while(look != nullptr)
		{
			
			if(look->used == true)
			{
				total_used_mem_blocks += 1;
				currently_used += look->size;
			}
			else
			{
				total_free_mem_blocks += 1;
				//currently_used -= look->size;
			}
			
			look = look->right;
			total_mem_blocks +=  1;
		}

		cout << "Maximum capacity of heap: 512B" << endl;
		cout << "Currently used memory (B): "  << currently_used << endl;
		cout << "Total memory blocks: "  << total_mem_blocks << endl;
		cout << "Total used memory blocks: " << total_used_mem_blocks << endl;
		cout << "Total free memory blocks: " << total_free_mem_blocks << endl;
		cout << "Fragmentation: " << get_fragmantation() << "% " << endl;
		cout << "------------------------------" << endl;
	
		memory_block* ptr = heap_begin;
		int ptr_index = 0;
		string trueOrFalse;
		while(ptr != nullptr)
		{
			if(ptr->used)
				trueOrFalse = "True";
			else
				trueOrFalse = "False";

			cout << "Block " << ptr_index;
			cout << "\t\tUsed: " << trueOrFalse ;
			cout << "\tSize (B): "<< ptr->size ;
			cout << "\tStarting Address: 0x" << hex << ptr->starting_address << dec << endl;
		
			ptr_index += 1;
			ptr = ptr->right;
		}
	}
	cout << "------------------------------" << endl;
	cout << "------------------------------" << endl;
}
/////////////////////////////////////////////////////////////
My_heap::~My_heap()
{
	int mem_leak = 0;
	while(heap_begin != nullptr)
	{
		if(heap_begin->used)
		{
			mem_leak += heap_begin->size;
		}
		memory_block* deleted_node = heap_begin;
		heap_begin = heap_begin->right;
		deleted_node = nullptr;
		delete deleted_node;
	}
	heap_begin = nullptr;
	blk = nullptr;

	cout << "At destruction, the heap had a memory leak of " << mem_leak << " bytes." << endl;
}