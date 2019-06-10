<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <p
    v-if="token"
    class="logging-in"
  >
    Logging in with Google...
  </p>
  <g-signin-button
    v-else
    :params="params"
    @success="onSigninSuccess"
    @error="onSigninError"
  >
    Sign in with Google
  </g-signin-button>
</template>

<script>
import GSigninButton from 'vue-google-signin-button'
import Vue from 'vue'
import conf from '../conf'

Vue.use(GSigninButton)

export default {
  name: 'SessionGoogle',
  inject: {
    ws: { from: 'kaizenWebSocket' }
  },
  data () {
    return {
      params: { client_id: conf.GOOGLE_ID },
      token: localStorage.getItem('kaizenGoogleToken')
    }
  },
  created () {
    if (this.token) {
      // Resend stored token if it expires no sooner than 10 seconds from now
      if (Date.now() + 10000 <
          parseInt(localStorage.getItem('kaizenGoogleTokenExpiresAt'))) {
        this.ws.sendFirst(this.token)
      } else {
        this.token = null
      }
    }
  },
  methods: {
    onSigninSuccess (googleUser) {
      const response = googleUser.getAuthResponse()
      this.token = 'GSI#' + response.id_token
      this.ws.sendFirst(this.token)
      localStorage.setItem('kaizenGoogleToken', this.token)
      localStorage.setItem('kaizenGoogleTokenExpiresAt', response.expires_at)
    },
    onSigninError (error) {
      console.error('Google Sign-in Error', error)
    }
  }
}

</script>

<style lang="sass" scoped>
@import "../conf.sass"

.logging-in,
.g-signin-button
  position: absolute
  top: 50%
  left: 50%
  transform: translate(-50%, -50%)

.g-signin-button
  padding: 1rem
  font-size: 1.5rem
  background-color: $color-bg-a
  cursor: pointer

</style>
