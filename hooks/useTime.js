// hooks/useTime.js - 时间显示自定义Hook

import { useState, useEffect } from 'react'

const useTime = () => {
  const [currentTime, setCurrentTime] = useState(null)
  const [mounted, setMounted] = useState(false)

  useEffect(() => {
    // 设置mounted状态，避免hydration错误
    setMounted(true)
    setCurrentTime(new Date())
    
    // 每秒更新时间
    const timer = setInterval(() => {
      setCurrentTime(new Date())
    }, 1000)
    
    return () => {
      clearInterval(timer)
    }
  }, [])

  return {
    currentTime,
    mounted
  }
}

export default useTime