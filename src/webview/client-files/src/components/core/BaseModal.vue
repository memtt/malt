<!----------------------------------------------------------
|    PROJECT  : MALT (MALoc Tracker)
|    DATE     : 11/2025
|    LICENSE  : CeCILL-C
|    FILE     : src/webview/client-files/src/components/core/BaseModal.vue
|-----------------------------------------------------------
|    AUTHOR   : Emeric GUYON - 2025
----------------------------------------------------------->
<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="modal-overlay" @click="handleOverlayClick">
        <div class="modal-container" :class="containerClass" @click.stop>
          <div v-if="!noHeader" class="modal-header" :class="headerClass">
            <slot name="header">
              <h2 v-if="title" class="modal-title">{{ title }}</h2>
            </slot>
            <div v-if="$slots['header-actions']" class="modal-header-actions">
              <slot name="header-actions"></slot>
            </div>
          </div>
          <button v-if="!noCloseButton" class="modal-close" @click="close">
            <IconClose />
          </button>

          <div class="modal-body" :class="bodyClass">
            <slot></slot>
          </div>

          <div v-if="$slots.footer" class="modal-footer" :class="footerClass">
            <slot name="footer"></slot>
          </div>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<script setup lang="ts">
import IconClose from '@/assets/icons/icon-close.svg?component'

interface Props {
  modelValue: boolean
  title?: string
  noHeader?: boolean
  noCloseButton?: boolean
  closeOnOverlayClick?: boolean
  containerClass?: string
  headerClass?: string
  bodyClass?: string
  footerClass?: string
}

const props = withDefaults(defineProps<Props>(), {
  title: '',
  noHeader: false,
  noCloseButton: false,
  closeOnOverlayClick: true,
  containerClass: '',
  headerClass: '',
  bodyClass: '',
  footerClass: '',
})

const emit = defineEmits<{
  'update:modelValue': [value: boolean]
}>()

const close = () => {
  emit('update:modelValue', false)
}

const handleOverlayClick = () => {
  if (props.closeOnOverlayClick) {
    close()
  }
}
</script>

<style scoped lang="scss">
@use '@/assets/scss/variables' as *;

.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 10000;
  padding: 20px;
}

.modal-container {
  position: relative;
  background: $color-bg-primary;
  border-radius: $radius-lg;
  max-width: 1200px;
  width: 100%;
  max-height: 90vh;
  overflow-y: auto;
  box-shadow: 0 10px 40px rgba(0, 0, 0, 0.2);
  display: flex;
  flex-direction: column;
  width: auto;
}

.modal-header {
  position: relative;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: $spacing-2xl $spacing-3xl $spacing-md;
  gap: $spacing-lg;
}

.modal-title {
  font-family: $font-family-base;
  font-size: $font-size-2xl;
  font-weight: $font-weight-normal;
  margin: 0;
  color: $color-primary;
}

.modal-header-actions {
  display: flex;
  align-items: center;
  gap: $spacing-sm;
  margin-left: auto;
  margin-right: $spacing-3xl;
}

.modal-close {
  position: absolute;
  top: 20px;
  right: 20px;
  width: 32px;
  height: 32px;
  border: none;
  background: transparent;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: $radius-md;
  transition: background $transition-base $transition-ease;
  z-index: 1;
}

.modal-close:hover {
  background: rgba(0, 0, 0, 0.05);
}

.modal-close svg {
  width: 18px;
  height: 18px;
  color: $color-text-dark;
}

.modal-body {
  padding: 0 $spacing-3xl $spacing-3xl;
  overflow-y: auto;
  flex: 1;
}

.modal-footer {
  display: flex;
  justify-content: flex-end;
  gap: $spacing-sm;
  padding: $spacing-lg $spacing-xl;
  border-top: 1px solid $color-border-gray;
}

/* Modal transitions */
.modal-enter-active,
.modal-leave-active {
  transition: opacity $transition-slow $transition-ease;
}

.modal-enter-from,
.modal-leave-to {
  opacity: 0;
}

.modal-enter-active .modal-container,
.modal-leave-active .modal-container {
  transition: transform $transition-slow $transition-ease;
}

.modal-enter-from .modal-container,
.modal-leave-to .modal-container {
  transform: scale(0.9);
}

/* Responsive */
@media (max-width: 768px) {
  .modal-header {
    padding: $spacing-xl $spacing-xl $spacing-md;
  }

  .modal-body {
    padding: 0 $spacing-xl $spacing-xl;
  }

  .modal-title {
    font-size: $font-size-xl;
  }
}
</style>
