// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit() // initialize allocator
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP); // 커널 끝과 phystop 사이 모든 페이지를 보유하게 free 목록을 초기화한다.
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p; // PTE는 4096바이트의 정렬된 pa 경계만 접근 가능
  p = (char*)PGROUNDUP((uint64)pa_start); // PGROUNDUP으로 정렬된 물리 주소만 free하게 보장한다.
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE); // 1로 초기화

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist; // freelist에 추가한다
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

uint64 get_free_memory(void){
  struct run * r;

  uint64 memsum = 0;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){
    r = r->next;
    memsum += PGSIZE;
  }
  release(&kmem.lock);
  return memsum;
}