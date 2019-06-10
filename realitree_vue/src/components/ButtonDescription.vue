<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <button-icon
    :title="value ? titleHide : titleShow"
    @click.native="$emit('input', !value)"
  >
    <path
      :class="{ hideDescription: value }"
      d="M14 17H4v2h10v-2zm6-8H4v2h16V9zM4 15h16v-2H4v2zM4 5v2h16V5H4z"
    />
  </button-icon>
</template>

<script>
import ButtonIcon from './ButtonIcon'

export default {
  name: 'ButtonDescription',
  components: {
    ButtonIcon
  },
  props: {
    value: {
      type: Boolean,
      required: true
    },
    titleHide: {
      type: String,
      required: true
    },
    titleShow: {
      type: String,
      required: true
    }
  }
}

</script>

<style lang="sass" scoped>
@import "../conf.sass"

path
  fill: $color-button-description-show

path.hideDescription
  fill: $color-button-description-hide

</style>
