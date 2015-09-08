// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
        if (!(err & FEC_WR)) panic("not a write");
        if (!(uvpt[PGNUM(addr)] & PTE_COW)) panic("not to a COW page");

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
        r = sys_page_alloc(0, (void*)PFTEMP, PTE_U|PTE_P|PTE_W);
        if (r != 0) panic("failed to allocate: %e", r);
        memmove((void*)PFTEMP, (void*)PTE_ADDR(addr), PGSIZE);
        r = sys_page_map(0, (void*)PFTEMP, 0, (void*)PTE_ADDR(addr), PTE_U|PTE_P|PTE_W);
        if (r != 0) panic("failed to map: %e", r);
        r = sys_page_unmap(0, (void*)PFTEMP);
        if (r != 0) panic("failed to ummap: %e", r);
	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	//panic("duppage not implemented");
        void *addr;

        // LAB 4: Your code here.
        pde_t pde;
        pte_t pte;
        addr = (void*)(pn * PGSIZE);
        pde = uvpd[PDX(addr)];
        if (!(pde & PTE_U) || !(pde & PTE_P)) return 0;
        pte = uvpt[pn];
        if (!(pte & PTE_U) || !(pte & PTE_P)) return 0;

        if ((pte & PTE_W) || (pte & PTE_COW))
        {
                r = sys_page_map(thisenv->env_id, addr, envid, addr, PTE_U|PTE_P|PTE_COW);
                if (r != 0) panic("failed to map at 1: %e", r);
                r = sys_page_map(envid, addr, thisenv->env_id, addr, PTE_U|PTE_P|PTE_COW);
                if (r != 0) panic("failed to map at 2: %e", r);
        }
        else
        {
                r = sys_page_map(thisenv->env_id, addr, envid, addr, PTE_U|PTE_P);
                if (r != 0) panic("failed to map at 3: %e", r);
        }
        return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	//panic("fork not implemented");
        int r, pn, epn;
        envid_t eid;

        set_pgfault_handler(pgfault);

        eid = sys_exofork();
        if (eid < 0) panic("failed to create child: %e", eid);

        if (eid == 0)
        {
               thisenv = &envs[ENVX(sys_getenvid())];
                return 0;
        }

        pn = PGNUM(UTEXT);
        epn = PGNUM(UXSTACKTOP - PGSIZE);
        for (; pn < epn; pn++)
                duppage(eid, pn);

        r = sys_page_alloc(eid, (void*)(UXSTACKTOP - PGSIZE), PTE_U|PTE_P|PTE_W);
        if (r < 0) panic("failed to allocate UXSTACK: %e", r);

        r = sys_env_set_pgfault_upcall(eid, thisenv->env_pgfault_upcall);
        if (r < 0) panic("failed to set upcall: %e", r);

        r = sys_env_set_status(eid, ENV_RUNNABLE);
        if (r < 0) panic("failed to set RUNNABLE: %e", r);

        return eid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}

