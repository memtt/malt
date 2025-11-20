<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/shared/DomainsTable.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="domains-table">
    <table class="table">
      <thead>
        <tr>
          <th class="col-domain">Domain</th>
          <th class="col-count">Count</th>
          <th class="col-mem">Cumulated allocations</th>
          <th class="col-description">Description</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td class="col-domain">C</td>
          <td class="col-count">{{ props.getHumanCount(props.domains?.counters.c || 0) }}</td>
          <td class="col-mem">{{ props.getHumanMem(props.domains?.mem.c || 0) }}</td>
          <td class="col-description">
            The memory number of allocations made in C/C++/Fortran/Rust code.
          </td>
        </tr>
        <tr>
          <td class="col-domain">Python Objects</td>
          <td class="col-count">{{ props.getHumanCount(props.domains?.counters.pyObj || 0) }}</td>
          <td class="col-mem">{{ props.getHumanMem(props.domains?.mem.pyObj || 0) }}</td>
          <td class="col-description">
            The memory number of allocations made in Python for objects. Note it uses a dedicated
            fast allocator.
          </td>
        </tr>
        <tr>
          <td class="col-domain">Python Mem</td>
          <td class="col-count">{{ props.getHumanCount(props.domains?.counters.pyMem || 0) }}</td>
          <td class="col-mem">{{ props.getHumanMem(props.domains?.mem.pyMem || 0) }}</td>
          <td class="col-description">
            The memory number of allocations made in Python for medium sizes.
          </td>
        </tr>
        <tr>
          <td class="col-domain">Python Raw</td>
          <td class="col-count">{{ props.getHumanCount(props.domains?.counters.pyRaw || 0) }}</td>
          <td class="col-mem">{{ props.getHumanMem(props.domains?.mem.pyRaw || 0) }}</td>
          <td class="col-description">
            The memory number of allocations made in Python for large allocations, sent directly to
            the C allocator.
          </td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script setup lang="ts">
import type { SummaryDomains } from '@/types/home';

interface Props {
  domains?: SummaryDomains;
  getHumanCount: (count: number) => string;
  getHumanMem: (bytes: number) => string;
}

const props = defineProps<Props>();
</script>

<style scoped>
.domains-table {
  width: 100%;
}

.table {
  width: 100%;
  border-collapse: collapse;
  border: 1px solid #dee2e6;
}

.table thead {
  background: #f8f9fa;
}

.table th {
  padding: 0.75rem;
  text-align: left;
  font-weight: 600;
  font-size: 0.875rem;
  color: #495057;
  border-bottom: 2px solid #dee2e6;
}

.table tbody tr {
  border-bottom: 1px solid #dee2e6;
}

.table tbody tr:hover {
  background: #f8f9fa;
}

.table td {
  padding: 0.75rem;
  font-size: 0.875rem;
}

.col-domain {
  width: 15%;
  font-weight: 500;
  color: #212529;
}

.col-count {
  width: 15%;
  text-align: right;
  font-family: 'Menlo', 'Monaco', 'Consolas', 'Courier New', monospace;
  color: #495057;
}

.col-mem {
  width: 20%;
  text-align: right;
  font-family: 'Menlo', 'Monaco', 'Consolas', 'Courier New', monospace;
  color: #495057;
}

.col-description {
  width: 50%;
  color: #6c757d;
  font-size: 0.8125rem;
}

/* Responsive */
@media (max-width: 768px) {
  .col-description {
    display: none;
  }

  .col-domain {
    width: 30%;
  }

  .col-count {
    width: 35%;
  }

  .col-mem {
    width: 35%;
  }
}
</style>
