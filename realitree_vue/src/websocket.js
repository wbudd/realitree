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

    this.sendKind = Object.freeze({
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

    this.receiveKind = Object.freeze({
      ALL_UPD: 0,
      PROJECT_ADD_ACK: 10,
      PROJECT_ADD_UPD: 11,
      PROJECT_MOVE_ACK: 12,
      PROJECT_MOVE_UPD: 13,
      PROJECT_TITLE_ACK: 14,
      PROJECT_TITLE_UPD: 15,
      PROJECT_DESCRIPTION_ACK: 16,
      PROJECT_DESCRIPTION_UPD: 17,
      PROJECT_COMPLETE_ACK: 18,
      PROJECT_COMPLETE_UPD: 19,
      PROJECT_ABORT_ACK: 20,
      PROJECT_ABORT_UPD: 21
      // todo
    })

    const ws = new WebSocket(conf.WS_URL)
    ws.binaryType = 'arraybuffer'
    ws.onmessage = ev => {
      if ((ev.data instanceof ArrayBuffer)) {
        let ba = new Uint8Array(ev.data)
        let dv = new DataView(ev.data)
        switch (ba[0]) {
        case ALL_UPD:
          this.newId = dv.getUint32(1)
          this.updateProjects(this.receiveProjects(ba, dv, 5))
          break
        case PROJECT_TITLE_ACK:
          break // todo
        case PROJECT_TITLE_UPD:
          break // todo
        case PROJECT_DESCRIPTION_ACK:
          break // todo
        case PROJECT_DESCRIPTION_UPD:
          break // todo
        default:
          break // todo
        }
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
                 int16 = [], int32 = [], uint8 = [], uint16 = [], uint32 = [],
                 str = [], strLog = [] }) {
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
    str.forEach(str => { i = this.setStr(msg, i, str) })
    strLog.forEach(strLog =>
      { i = this.setStrUpdate(msg, dv, i, strLog.getUpdate()) })
    console.log(msg)
    this.sendRaw(msg)
  }

  setStr (msg, i, str) {
    dv.setUint16(i, getUtf8ByteCount(insertedStr); i += 2
    return encodeUtf8Buf(insertedStr, msg, i)
  }

  setStrUpdate (msg, dv, i,
    {crc, spliceCodepointIndex, deletedCodepointCount, insertedStr}) {
    dv.setUint32(i, crc); i += 4
    dv.setUint16(i, spliceCodepointIndex); i += 2
    dv.setUint16(i, deletedCodepointCount); i += 2
    return this.setStr(msg, i, insertedStr)
  }

  receiveProjects (ba, dv, i) {
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
      kind: this.sendKind.PROJECT_ADD,
      id: [parentProjectId, newProjectId]
    })
  }

  sendProjectAbort (parentProjectId, i) {
    // change to (id)
    this.send({
      kind: this.sendKind.PROJECT_ABORT,
      id: parentProjectId,
      uint16: i
    })
  }

  sendProjectMove (sourceParentId, targetParentId, iSource, iTarget) {
    // change to (projectId, targetParentId, iTarget)
    this.send({
      kind: this.sendKind.PROJECT_MOVE,
      id: [sourceParentId, targetParentId],
      uint16: [iSource, iTarget]
    })
  }

  sendProjectCollapsed (id, isCollapsed) {
    this.send({
      kind: this.sendKind.PROJECT_COLLAPSE,
      id: id,
      uint8: isCollapsed
    })
  }

  sendProjectTitle (id, oldTitle], [...newTitle]) {
    // change to (id, CrcOfOldTitle, spliceIndex, deleteCount, newString)
    this.send({
      kind: this.sendKind.PROJECT_TITLE,
      uint32: crc32(oldTitle),
      id: id,
      str: newTitle
    })
  }

  sendProjectDescription (id, oldDescription, newDescription) {
    // change to (id, CrcOfOldDescription, spliceIndex, deleteCount, newString)
    this.send({
      kind: this.sendKind.PROJECT_DESCRIPTION,
      id: id,
      str: newDescription
    })
  }
}
