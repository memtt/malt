/**
 * Composable for Global Variables Top List
 * Handles top 10 variables with search functionality across all variables
 */

import { computed, ref, type Ref } from 'vue'
import { getAllVariables } from '@/utils/globalVars/calculators'
import type { GlobalVarsData } from '@/types/global-vars'

export function useGlobalVarsTopList(
  data: Ref<GlobalVarsData | undefined>,
  maxThreadCount: Ref<number>,
) {
  const searchQuery = ref('')

  // Get all variables sorted by size
  const allVariables = computed(() => {
    return getAllVariables(data.value ?? null, maxThreadCount.value)
  })

  // Filter variables based on search query, then limit to top 10 of filtered results
  const filteredVariables = computed(() => {
    const query = searchQuery.value.trim().toLowerCase()

    if (!query) {
      // No search query: return top 10
      return allVariables.value.slice(0, 10)
    }

    // Search across all variables, then return top 10 matches
    return allVariables.value
      .filter((variable) => variable.name.toLowerCase().includes(query))
      .slice(0, 10)
  })

  return {
    searchQuery,
    allVariables,
    filteredVariables,
  }
}
