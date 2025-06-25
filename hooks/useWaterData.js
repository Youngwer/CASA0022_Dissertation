// hooks/useWaterData.js - 水质数据获取自定义Hook

import { useState, useEffect } from 'react'

const useWaterData = () => {
  const [waterData, setWaterData] = useState({
    temperature: 0,
    ph: 0,
    turbidity: 0,
    conductivity: 0,
    tds: 0,
    status: 'UNKNOWN',
    lastUpdate: 'Loading...'
  })
  
  const [dataSource, setDataSource] = useState('')
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState(null)
  const [isOnline, setIsOnline] = useState(false)

  // 获取水质数据
  const fetchWaterData = async () => {
    try {
      setLoading(true)
      setError(null)
      
      console.log('🚀 Fetching water quality data...')
      
      const response = await fetch('/api/water-data')
      const result = await response.json()

      console.log('📦 API Response:', result)

      if (result.success && result.data) {
        setWaterData({
          temperature: result.data.temperature || 0,
          ph: result.data.ph || 0,
          turbidity: result.data.turbidity || 0,
          conductivity: result.data.conductivity || 0,
          tds: result.data.tds || 0,
          status: result.data.status || 'UNKNOWN',
          lastUpdate: result.data.lastUpdate || 'Unknown'
        })
        setDataSource(result.source)
        setIsOnline(true)
        console.log('✅ Data loaded successfully')
      } else {
        throw new Error(result.message || 'Failed to load data')
      }
    } catch (err) {
      console.error('❌ Error loading data:', err)
      setError(`Failed to load data: ${err.message}`)
      setIsOnline(false)
    } finally {
      setLoading(false)
    }
  }

  // 初始化数据加载和定时器
  useEffect(() => {
    // 初始加载数据
    fetchWaterData()
    
    // 每30秒自动刷新数据
    const dataTimer = setInterval(fetchWaterData, 30000)
    
    return () => {
      clearInterval(dataTimer)
    }
  }, [])

  return {
    waterData,
    dataSource,
    loading,
    error,
    isOnline,
    fetchWaterData,
    refreshData: fetchWaterData
  }
}

export default useWaterData