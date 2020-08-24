# buddy_allocator

This is a simple implementation of buddy allocator. Buddy allocator is implemented in Linux kernel. The mechanism works as follows. We have a list of linked list. Each linked list manage the free region of the memory of size exactly two to the power. If we allocate memory, then we find if the closest two to the power list is available. If it's available, we then allocate it. If it is not avaiable, we try to tear the bigger region into smaller region (meaning split the "bigger" list into "smaller" list).

On deletion, we just merge two region if they are from the same bigger region (exactly one upper level) and merge them.
