<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <button-icon
    :title="title"
  >
    <path
      d="M19 13h-6v6h-2v-6H5v-2h6V5h2v6h6v2z"
    />
  </button-icon>
</template>

<script>
import ButtonIcon from './ButtonIcon'

export default {
  name: 'ButtonAdd',
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
  fill: $color-button-add

</style>
