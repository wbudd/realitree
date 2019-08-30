// SPDX-License-Identifier: MIT
// Copyright © 2019 William Budd

import conf from './conf'

export default class {
  constructor () {
    this.newId = 1

    this.draggedProjectTarget = {
      parentId: -1,
      index: -1
    }

    this.kind = Object.freeze({
      ALL: 0,
      PROJECT_ADD: 10,
      PROJECT_MOVE: 11,
      PROJECT_COLLAPSE: 12,
      PROJECT_TITLE: 13,
      PROJECT_DESCRIPTION: 14,
      PROJECT_COMPLETE: 15,
      PROJECT_ABORT: 16,
      TASK_ADD: 50,
      TASK_MOVE: 51,
      TASK_DESCRIPTION: 52,
      TASK_START_TIME: 53,
      TASK_END_TIME: 54,
      TASK_TAG_PROJECT: 55,
      TASK_UNTAG_PROJECT: 56,
      TASK_ACTIVATE: 57,
      TASK_DEACTIVATE: 58,
      TASK_COMPLETE: 59,
      TASK_ABORT: 60,
      TASK_AFTERWORD: 61
    })

    const ws = new WebSocket(conf.WS_URL)
    ws.binaryType = 'arraybuffer'
    ws.onmessage = ev => {
      if ((ev.data instanceof ArrayBuffer)) {
        let ba = new Uint8Array(ev.data)
        let dv = new DataView(ev.data)
        this.newId = dv.getUint32(0)
        this.updateProjects(this.readProjects(ba, dv, 4))
      } else {
        console.error('Unexpectedly received non-binary WebSocket data.')
      }
    }
    ws.onclose = ev => {
      console.warn('The WebSocket was closed', ev)
    }
    ws.onerror = ev => {
      console.error('WebSocket error', ev)
    }
    this.sendRaw = ws.send.bind(ws)
    this.sendFirst = msgFirst => {
      const f = () => ws.send(msgFirst)
      ws.readyState === 1 ? f() : ws.onopen = f
    }
  }

  send (args) {
    Object.keys(args).forEach(key => {
      const val = args[key]
      if (!Array.isArray(val)) {
        args[key] = [val]
      }
    })
    this.send_(args)
  }
  send_ ({ kind, id = [], bool = [], float32 = [], float64 = [], int8 = [],
    int16 = [], int32 = [], uint8 = [], uint16 = [], uint32 = [], str = [] }) {
    const size = 1 +
      4 * id.length +
      bool.length +
      4 * float32.length +
      8 * float64.length +
      int8.length +
      2 * int16.length +
      4 * int32.length +
      uint8.length +
      2 * uint16.length +
      4 * uint32.length +
      str.reduce((sSize, s) => sSize + this.getUTF8Size(s), 0)
    let msg = new Uint8Array(size)
    let dv = new DataView(msg.buffer)
    msg[0] = kind
    let i = 1
    id.forEach(id => { dv.setUint32(i, id); i += 4 })
    bool.forEach(bool => { msg[i++] = bool })
    float32.forEach(float32 => { dv.setFloat32(i, float32); i += 4 })
    float64.forEach(float64 => { dv.setFloat64(i, float64); i += 8 })
    int8.forEach(int8 => { dv.setInt8(i++, int8) })
    int16.forEach(int16 => { dv.setInt16(i, int16); i += 2 })
    int32.forEach(int32 => { dv.setInt32(i, int32); i += 4 })
    uint8.forEach(uint8 => { msg[i++] = uint8 })
    uint16.forEach(uint16 => { dv.setUint16(i, uint16); i += 2 })
    uint32.forEach(uint32 => { dv.setUint32(i, uint32); i += 4 })
    str.forEach(str => { i = this.setUTF8(msg, i, str) })
    console.log(msg)
    this.sendRaw(msg)
  }

  getUTF8Size (str) {
    let size = 0
    for (let i = 0; i < str.length; i++) {
      let c = str.charCodeAt(i)
      if (c < 128) {
        size++
      } else if (c < 2048) {
        size += 2
      } else if (((c & 0xFC00) === 0xD800) && (i + 1) < str.length &&
                 ((str.charCodeAt(i + 1) & 0xFC00) === 0xDC00)) {
        size += 4
        i++
      } else {
        size += 3
      }
    }
    return size
  }

  getUTF8 (msg, i = 0) {
    let str = ''
    for (;;) {
      let ch = msg[i++]
      if (!ch) { // Strings are '\0' byte terminated
        return [str, i]
      }
      switch (ch >> 4) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: {
          str += String.fromCharCode(ch)
          break
        }
        case 12: case 13: {
          str += String.fromCharCode(((ch & 0x1F) << 6) | (msg[i++] & 0x3F))
          break
        }
        case 14: {
          let ch2 = msg[i++]
          str += String.fromCharCode(((ch & 0x0F) << 12) |
                                     ((ch2 & 0x3F) << 6) |
                                     ((msg[i++] & 0x3F) << 0))
        }
      }
    }
  }

  setUTF8 (msg, i, str) {
    for (let j = 0; j < str.length; j++) {
      let c = str.charCodeAt(j)
      if (c < 128) {
        msg[i++] = c
      } else if (c < 2048) {
        msg[i++] = (c >> 6) | 192
        msg[i++] = (c & 63) | 128
      } else if (((c & 0xFC00) === 0xD800) && (j + 1) < str.length &&
                 ((str.charCodeAt(j + 1) & 0xFC00) === 0xDC00)) {
        c = 0x10000 + ((c & 0x03FF) << 10) + (str.charCodeAt(++j) & 0x03FF)
        msg[i++] = (c >> 18) | 240
        msg[i++] = ((c >> 12) & 63) | 128
        msg[i++] = ((c >> 6) & 63) | 128
        msg[i++] = (c & 63) | 128
      } else {
        msg[i++] = (c >> 12) | 224
        msg[i++] = ((c >> 6) & 63) | 128
        msg[i++] = (c & 63) | 128
      }
    }
    return i
  }

  readProjects (ba, dv, i) {
    let getProjects = () => {
      let projects = []
      const childCount = dv.getUint16(i)
      i += 2
      for (let j = 0; j < childCount; j++) {
        let p = {
          id: dv.getUint32(i),
          isDraggable: false,
          isTagged: false // todo
        }
        i += 4
        {
          const [str, j] = this.getUTF8(ba, i)
          p.title = str
          i = j
        }
        {
          const [str, j] = this.getUTF8(ba, i)
          p.description = str
          i = j
        }
        p.isCollapsed = ba[i++] === 1
        p.children = getProjects(p)
        projects.push(p)
      }
      return projects
    }
    return getProjects()
  }

  sendProjectAdd (parentProjectId, newProjectId) {
    this.send({
      kind: this.kind.PROJECT_ADD,
      id: [parentProjectId, newProjectId]
    })
  }

  sendProjectAbort (parentProjectId, i) {
    // change to (id)
    this.send({
      kind: this.kind.PROJECT_ABORT,
      id: parentProjectId,
      uint16: i
    })
  }

  sendProjectMove (sourceParentId, targetParentId, iSource, iTarget) {
    // change to (projectId, targetParentId, iTarget)
    this.send({
      kind: this.kind.PROJECT_MOVE,
      id: [sourceParentId, targetParentId],
      uint16: [iSource, iTarget]
    })
  }

  sendProjectCollapsed (id, isCollapsed) {
    this.send({
      kind: this.kind.PROJECT_COLLAPSE,
      id: id,
      uint8: isCollapsed
    })
  }

  sendProjectTitle (id, title) {
    // change to (id, CrcOfOldTitle, spliceIndex, deleteCount, newString)
    this.send({
      kind: this.kind.PROJECT_TITLE,
      id: id,
      str: title
    })
  }

  sendProjectDescription (id, description) {
    // change to (id, CrcOfOldDescription, spliceIndex, deleteCount, newString)
    this.send({
      kind: this.kind.PROJECT_DESCRIPTION,
      id: id,
      str: description
    })
  }
}
