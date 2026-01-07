<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/home/SummaryStatsTable.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <div class="summary-stats-table">
    <!-- Toggle buttons -->
    <div class="toggle-buttons">
      <button class="btn-toggle" @click="toggleDetails">
        {{ displayLevel === 1 ? 'Show all details' : 'Hide details' }}
      </button>
      <button class="btn-toggle" @click="toggleHelp">
        {{ showHelp ? 'Hide help' : 'Show help' }}
      </button>
    </div>

    <!-- Stats table -->
    <table class="stats-table">
      <tbody>
        <tr
          v-for="(entry, index) in filteredEntries"
          :key="index"
          :class="{ 'has-warning': entry.key && props.hasSummaryWarnings(entry.key) }"
        >
          <!-- Separator row -->
          <template v-if="entry.type === 'separator'">
            <td colspan="3" class="separator-row"></td>
          </template>

          <!-- Normal row -->
          <template v-else>
            <td class="col-name">{{ entry.name }}</td>
            <td class="col-value">
              {{ props.getFormattedValue(entry) }}
              <span
                v-if="entry.key && props.warnings[entry.key]"
                v-for="(warning, wIdx) in props.warnings[entry.key]"
                :key="wIdx"
                class="warning-icon"
                :title="warning"
              >
                ⚠
              </span>
            </td>
            <td v-if="showHelp" class="col-help">{{ entry.help }}</td>
          </template>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';
import type { HomeSummaryData, SummaryViewEntry, SummaryWarnings } from '@/types/home';

interface Props {
  entries: SummaryViewEntry[];
  data?: HomeSummaryData;
  warnings: SummaryWarnings;
  getFormattedValue: (entry: SummaryViewEntry) => string;
  hasSummaryWarnings: (key: string) => boolean;
}

const props = defineProps<Props>();

// State
const displayLevel = ref<number>(1);
const showHelp = ref<boolean>(false);

/**
 * Toggle details (level 1 or 2)
 */
const toggleDetails = () => {
  displayLevel.value = displayLevel.value === 1 ? 2 : 1;
};

/**
 * Toggle help visibility
 */
const toggleHelp = () => {
  showHelp.value = !showHelp.value;
};

/**
 * Filter entries by display level
 * Show entry if:
 * - It's a separator with level <= displayLevel
 * - It has level <= displayLevel
 * - It has warnings (always show)
 */
const filteredEntries = computed(() => {
  return props.entries.filter((entry) => {
    // Separators
    if (entry.type === 'separator') {
      return entry.level <= displayLevel.value;
    }

    // Normal entries
    if (entry.level <= displayLevel.value) {
      return true;
    }

    // Always show if has warnings
    if (entry.key && props.hasSummaryWarnings(entry.key)) {
      return true;
    }

    return false;
  });
});
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.summary-stats-table {
  width: 100%;
}

/* Toggle buttons */
.toggle-buttons {
  display: flex;
  gap: 0.5rem;
  margin-bottom: 1rem;
}

.btn-toggle {
  padding: 0.5rem 1rem;
  background: #007bff;
  color: white;
  border: none;
  border-radius: $spacing-xs;
  font-size: $font-size-sm;
  cursor: pointer;
  transition: background-color $transition-fast ease;
}

.btn-toggle:hover {
  background: #0056b3;
}

.btn-toggle:active {
  transform: scale(0.98);
}

/* Stats table */
.stats-table {
  width: 100%;
  border-collapse: collapse;
  border: 1px solid $gray-200;
}

.stats-table tbody tr {
  border-bottom: 1px solid $gray-200;
}

.stats-table tbody tr.has-warning {
  background: #fff3cd;
}

.stats-table td {
  padding: 0.75rem;
  font-size: $font-size-sm;
}

.col-name {
  width: 40%;
  font-weight: $font-weight-medium;
  color: $gray-900;
}

.col-value {
  width: 30%;
  text-align: right;
  font-family: $font-family-mono;
  color: $gray-600;
}

.col-help {
  width: 30%;
  color: $color-text-muted;
  font-size: 0.8125rem;
  font-style: italic;
}

/* Separator */
.separator-row {
  height: 0.5rem;
  background: $gray-50;
  padding: 0;
}

/* Warning icon */
.warning-icon {
  display: inline-block;
  margin-left: 0.5rem;
  color: #ff9800;
  font-size: 1.25rem;
  cursor: help;
}

/* Responsive */
@media (max-width: 768px) {
  .col-name {
    width: 50%;
  }

  .col-value {
    width: 50%;
  }

  .col-help {
    display: none;
  }

  .toggle-buttons {
    flex-direction: column;
  }
}
</style>
