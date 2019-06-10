// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

import Vue from 'vue'
import vClickOutside from 'v-click-outside'
import App from './App.vue'

Vue.use(vClickOutside)

Vue.config.productionTip = false

new Vue({
  render: h => h(App)
}).$mount('#app')
