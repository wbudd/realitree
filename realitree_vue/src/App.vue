<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <div
    id="app"
  >
    <agenda/>
  </div>
</template>

<script>
import Agenda from './components/Agenda'
import RealitreeWebSocket from './websocket'

export default {
  name: 'Realitree',
  components: {
    Agenda
  },
  data () {
    return {
      ws: new RealitreeWebSocket()
    }
  },
  beforeCreate () {
    document.body.setAttribute('spellcheck', false)
  },
  provide () {
    return {
      realitreeWebSocket: this.ws
    }
  }
}
</script>

<style lang="sass">
@import "./conf.sass"

html
  font-family: 'Avenir', Helvetica, Arial, sans-serif
  -webkit-font-smoothing: antialiased
  -moz-osx-font-smoothing: grayscale
  font-size: 62.5%

body
  color: $color-fg-a
  background-color: $color-bg-c

.bgColorMain
  background-color: $color-bg-b !important
.bgColorAlt
  background-color: $color-bg-c !important

body, ul, li, h1, h2, h3, h4, h5, p
  padding: 0
  margin: 0

ul
  list-style: none

li
  display: block

h1, h2, h3, h4, h5, th
  font-weight: normal

</style>
