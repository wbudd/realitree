<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <button-icon
    v-if="value"
    :title="titleExpand"
    @click.native="$emit('input', false)"
  >
    <path
      d="M16.59 8.59L12 13.17 7.41 8.59 6 10l6 6 6-6z"
    />
  </button-icon>
  <button-icon
    v-else
    :title="titleCollapse"
    @click.native="$emit('input', true)"
  >
    <path
      d="M12 8l-6 6 1.41 1.41L12 10.83l4.59 4.58L18 14z"
    />
  </button-icon>
</template>

<script>
import ButtonIcon from './ButtonIcon'

export default {
  name: 'ButtonCollapseExpand',
  components: {
    ButtonIcon
  },
  props: {
    value: {
      type: Boolean,
      required: true
    },
    titleCollapse: {
      type: String,
      required: true
    },
    titleExpand: {
      type: String,
      required: true
    }
  }
}

</script>

<style lang="sass" scoped>
@import "../conf.sass"

path
  fill: $color-button-collapse-expand

</style>
