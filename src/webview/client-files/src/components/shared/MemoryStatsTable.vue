<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/shared/MemoryStatsTable.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="memory-stats-table">
    <!-- Empty state -->
    <div v-if="!details" class="empty-state">
      <p>No data selected</p>
    </div>

    <!-- Tables -->
    <div v-else>
      <!-- Inclusive table -->
      <table class="stats-table">
        <thead>
          <tr>
            <th colspan="2">Inclusive</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Allocated memory</td>
            <td>{{ formatValue(details.total?.alloc.sum, 'B') }}</td>
          </tr>
          <tr>
            <td>Freed memory</td>
            <td>{{ formatValue(details.total?.free.sum, 'B') }}</td>
          </tr>
          <tr>
            <td>Local peak</td>
            <td>{{ formatValue(details.total?.maxAliveReq, 'B') }}</td>
          </tr>
          <tr>
            <td>Leaks</td>
            <td>{{ formatValue(details.total?.aliveReq, 'B') }}</td>
          </tr>
          <tr>
            <td>{{ formatValue(details.total?.alloc.count, '') }} alloc</td>
            <td>
              [ {{ formatValue(details.total?.alloc.min, 'B') }} ,
              {{ formatMean(details.total?.alloc.sum, details.total?.alloc.count, 'B') }} ,
              {{ formatValue(details.total?.alloc.max, 'B') }} ]
            </td>
          </tr>
          <tr>
            <td>{{ formatValue(details.total?.free.count, '') }} free</td>
            <td>
              [ {{ formatValue(details.total?.free.min, 'B') }} ,
              {{ formatMean(details.total?.free.sum, details.total?.free.count, 'B') }} ,
              {{ formatValue(details.total?.free.max, 'B') }} ]
            </td>
          </tr>
          <tr>
            <td>Lifetime</td>
            <td>
              [ {{ formatValue(details.total?.lifetime.min, '') }} ,
              {{ formatMean(details.total?.lifetime.sum, details.total?.lifetime.count, '') }} ,
              {{ formatValue(details.total?.lifetime.max, '') }} ] (cycles)
            </td>
          </tr>
        </tbody>
      </table>

      <!-- Exclusive table -->
      <table v-if="hasExclusiveValues" class="stats-table">
        <thead>
          <tr>
            <th colspan="2">Exclusive</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <td>Allocated memory</td>
            <td>{{ formatValue(details.own?.alloc.sum, 'B') }}</td>
          </tr>
          <tr>
            <td>Freed memory</td>
            <td>{{ formatValue(details.own?.free.sum, 'B') }}</td>
          </tr>
          <tr>
            <td>Local peak</td>
            <td>{{ formatValue(details.own?.maxAliveReq, 'B') }}</td>
          </tr>
          <tr>
            <td>Leaks</td>
            <td>{{ formatValue(details.own?.aliveReq, 'B') }}</td>
          </tr>
          <tr>
            <td>{{ formatValue(details.own?.alloc.count, '') }} alloc</td>
            <td>
              [ {{ formatValue(details.own?.alloc.min, 'B') }} ,
              {{ formatMean(details.own?.alloc.sum, details.own?.alloc.count, 'B') }} ,
              {{ formatValue(details.own?.alloc.max, 'B') }} ]
            </td>
          </tr>
          <tr>
            <td>{{ formatValue(details.own?.free.count, '') }} free</td>
            <td>
              [ {{ formatValue(details.own?.free.min, 'B') }} ,
              {{ formatMean(details.own?.free.sum, details.own?.free.count, 'B') }} ,
              {{ formatValue(details.own?.free.max, 'B') }} ]
            </td>
          </tr>
          <tr>
            <td>Lifetime</td>
            <td>
              [ {{ formatValue(details.own?.lifetime.min, '') }} ,
              {{ formatMean(details.own?.lifetime.sum, details.own?.lifetime.count, '') }} ,
              {{ formatValue(details.own?.lifetime.max, '') }} ] (cycles)
            </td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import { humanReadable } from '@/lib/helpers';
import type { SourceAnnotation } from '@/types/sources';

interface Props {
  details: SourceAnnotation | null;
}

const props = defineProps<Props>();

/**
 * Format value with unit
 */
const formatValue = (value: number | undefined, unit: string): string => {
  if (value === undefined) {
    return humanReadable(0, 1, unit, true);
  }
  return humanReadable(value, 1, unit, true);
};

/**
 * Format mean (rounded ratio)
 */
const formatMean = (
  sum: number | undefined,
  count: number | undefined,
  unit: string,
): string => {
  if (sum === undefined || count === undefined || count === 0) {
    return humanReadable(0, 1, unit, true);
  }
  const mean = Math.round(sum / count);
  return humanReadable(mean, 1, unit, true);
};

/**
 * Check if exclusive values exist
 */
const hasExclusiveValues = computed(() => {
  if (!props.details || !props.details.own) {
    return false;
  }

  const own = props.details.own;
  return own.alloc.count > 0 || own.free.count > 0;
});
</script>

<style scoped>
.memory-stats-table {
  padding: 1rem;
  background: white;
  border: 1px solid #dee2e6;
  border-radius: 0.25rem;
  overflow-y: auto;
  max-height: 100%;
}

.empty-state {
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 2rem;
  color: #6c757d;
  font-style: italic;
}

.stats-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 0.875rem;
  margin-bottom: 1.5rem;
}

.stats-table:last-child {
  margin-bottom: 0;
}

.stats-table thead {
  background: #f8f9fa;
}

.stats-table th {
  padding: 0.75rem;
  text-align: left;
  font-weight: 600;
  border: 1px solid #dee2e6;
}

.stats-table td {
  padding: 0.5rem 0.75rem;
  border: 1px solid #dee2e6;
}

.stats-table tbody tr:nth-child(odd) {
  background-color: #f8f9fa;
}

.stats-table tbody tr:hover {
  background-color: #e9ecef;
}

.stats-table td:first-child {
  font-weight: 500;
  color: #495057;
}

.stats-table td:last-child {
  font-family: 'Menlo', 'Monaco', 'Consolas', 'Courier New', monospace;
  font-size: 0.8rem;
  color: #212529;
}
</style>
