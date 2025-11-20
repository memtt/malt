import type { HelpSection } from '@/components/home/HelpModal.vue'
import IconRules from '@/assets/icons/icon-rules.svg?component'
import IconWeight from '@/assets/icons/icon-weight.svg?component'
import IconChrono from '@/assets/icons/icon-chrono.svg?component'
import IconStack from '@/assets/icons/icon-stack.svg?component'
import IconVariable from '@/assets/icons/icon-variable.svg?component'
import IconWater from '@/assets/icons/icon-water.svg?component'

export function useHelpSections(): HelpSection[] {
  return [
    {
      title: 'Peak',
      icon: IconRules,
      items: [
        {
          label: 'Physical memory peak',
          description:
            'Peak of total physical memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.',
        },
        {
          label: 'Virtual memory peak',
          description:
            'Peak of total virtual memory obtained by tracking /proc/self/statm. It contain all allocated objects, global variables and binary code.',
        },
        {
          label: 'Requested memory peak',
          description:
            'Peak of requested memory managed by malloc, free... It contain all dynamically allocated objects.',
        },
        {
          label: 'Alive object peak',
          description: 'Peak number of alive object allocated by malloc, calloc, realloc....',
        },
        {
          label: 'MALT peak memory',
          description: 'Peak of memory internally used by MALT.',
        },
      ],
    },
    {
      title: 'Allocator pressure',
      icon: IconWeight,
      items: [
        {
          label: 'Cumulated memory allocations',
          description:
            'Sum of all dynamic memory allocation througth malloc, calloc, realloc....',
        },
        {
          label: 'Allocation count',
          description:
            'Total number of all dynamic memory allocation with malloc, calloc, realloc....',
        },
        {
          label: 'Recycling ratio',
          description:
            'Ratio of cumulated memory allocations over maximum requested memory. Ideally it might be 1 if the program allocate the memory on start and free on end.',
        },
        {
          label: 'Smallest allocations',
          description: 'The smallest request size received by malloc, calloc...',
        },
        {
          label: 'Mean allocations',
          description: 'The mean request size received by malloc, calloc...',
        },
        {
          label: 'Largest allocations',
          description: 'The largest request size received by malloc, calloc...',
        },
      ],
    },
    {
      title: 'Allocator pressure',
      icon: IconChrono,
      items: [
        {
          label: 'Peak allocation rate',
          description: 'Peak of memory allocation rate (in memory size).',
        },
        {
          label: 'Peak allocation call rate',
          description: 'Peak of memory allocation call rate.',
        },
      ],
    },
    {
      title: 'Call stacks',
      icon: IconStack,
      items: [
        {
          label: 'Largest stack',
          description: 'Memory used by the largest stack seen during execution of threads.',
        },
      ],
    },
    {
      title: 'Variables globales / TLS',
      icon: IconVariable,
      items: [
        {
          label: 'Global variables',
          description: 'Memory used by global variables.',
        },
        {
          label: 'TLS variables',
          description:
            'Memory used by TLS variables (already multiplied by number of thread of execution).',
        },
        {
          label: 'Global variable count',
          description:
            'Total number of global and TLS variables from your application and libraries.',
        },
      ],
    },
    {
      title: 'Memory leak',
      icon: IconWater,
      items: [
        {
          label: 'Leaked memory',
          description: 'Total memory which has leaked (malloc, calloc... without free).',
        },
        {
          label: 'Leaked object count',
          description:
            'Total number of allocated object which have leaked (malloc, calloc... without free).',
        },
      ],
    },
  ]
}
