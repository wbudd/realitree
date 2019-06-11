<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <ul>
    <li
      v-for="task in tasks"
      :key="task.id"
    >
      {{ task.description }}
    </li>
  </ul>
</template>

<script>
export default {
  name: 'TaskList',
  inject: {
    ws: { from: 'realitreeWebSocket' }
  },
  props: {
    tasks: {
      type: Array,
      required: true
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

li
  padding: 1rem
  background-color: $color-bg-b

li:nth-child(even)
  background-color: $color-bg-c

</style>
