<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <button-icon
    :title="title"
  >
    <path
      d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"
    />
  </button-icon>
</template>

<script>
import ButtonIcon from './ButtonIcon'

export default {
  name: 'ButtonDelete',
  components: {
    ButtonIcon
  },
  props: {
    title: {
      type: String,
      required: true
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

path
  fill: $color-button-delete

</style>
