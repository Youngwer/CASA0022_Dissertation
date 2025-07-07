// pages/api/water-data.js - 支持水质标签的修复版本

import WaterQualityDB from '../../lib/database'

export default async function handler(req, res) {
  if (req.method !== 'GET') {
    return res.status(405).json({ message: 'Method not allowed' })
  }

  try {
    console.log('🚀 Fetching water data from Neon database...')
    
    // 检查 WaterQualityDB 是否正确导入
    if (!WaterQualityDB) {
      console.error('❌ WaterQualityDB not imported correctly')
      return res.status(500).json({
        success: false,
        message: 'Database module not available',
        source: 'error'
      })
    }

    // 初始化数据库连接
    await WaterQualityDB.initializeDatabase()
    const dbConnected = await WaterQualityDB.testConnection()

    if (!dbConnected) {
      console.log('❌ Database connection failed, using mock data')
      return res.status(200).json({
        success: true,
        data: getMockData(),
        source: 'mock',
        message: 'Database connection failed',
        timestamp: new Date().toISOString()
      })
    }

    const DEVICE_ID = process.env.DEVICE_ID || 'water-monitor'
    
    // 优先从数据库获取最新数据
    const latestFromDB = await WaterQualityDB.getLatestReading(DEVICE_ID)
    
    if (latestFromDB) {
      // 检查数据是否较新 (15分钟内)
      const dataAge = new Date() - new Date(latestFromDB.recorded_at)
      const isRecentData = dataAge < 15 * 60 * 1000 // 15分钟
      
      const waterData = formatDatabaseData(latestFromDB)
      
      let dataSource, message
      if (isRecentData) {
        dataSource = 'neon_database'
        message = `Fresh database data (${Math.round(dataAge / 1000)}s ago)`
      } else {
        dataSource = 'neon_database_old'
        message = `Older database data (${Math.round(dataAge / 60000)}min ago)`
      }

      console.log(`✅ Database response: ${dataSource} | ${waterData?.temperature}°C, pH ${waterData?.ph}, Status: ${waterData?.status}`)
      
      return res.status(200).json({
        success: true,
        data: waterData,
        source: dataSource,
        message: message,
        timestamp: new Date().toISOString(),
        data_age_seconds: Math.round(dataAge / 1000)
      })
    } else {
      // 数据库中没有数据，返回模拟数据
      console.log('⚠️ No data in database, using mock data')
      return res.status(200).json({
        success: true,
        data: getMockData(),
        source: 'mock',
        message: 'No real data available',
        timestamp: new Date().toISOString()
      })
    }

  } catch (error) {
    console.error('💥 API Error:', error)
    return res.status(500).json({
      success: false,
      message: 'Internal server error',
      error: error.message,
      source: 'error'
    })
  }
}

// 格式化数据库数据（包含水质标签）
function formatDatabaseData(dbData) {
  if (!dbData) return getMockData()
  
  const safeParseFloat = (value, defaultValue) => {
    const parsed = parseFloat(value)
    return isNaN(parsed) ? defaultValue : parsed
  }

  const waterData = {
    temperature: safeParseFloat(dbData.temperature, 0),
    ph: safeParseFloat(dbData.ph, 7),
    turbidity: safeParseFloat(dbData.turbidity, 0),
    conductivity: safeParseFloat(dbData.conductivity, 0),
    tds: safeParseFloat(dbData.tds, 0),
    status: dbData.status || 'UNKNOWN',
    waterLabel: dbData.water_label || null, // 包含水质标签
    lastUpdate: new Date(dbData.recorded_at).toLocaleString('en-GB', { 
      timeZone: 'Europe/London',
      year: 'numeric', 
      month: 'short', 
      day: 'numeric',
      hour: '2-digit', 
      minute: '2-digit'
    })
  }

  return waterData
}

// 模拟数据（包含水质标签）
function getMockData() {
  return {
    temperature: 22.5,
    ph: 7.2,
    turbidity: 0.8,
    conductivity: 350.0,
    tds: 280.0,
    status: 'EXCELLENT',
    waterLabel: 'Demo Sample Water', // 模拟标签
    lastUpdate: new Date().toLocaleString('en-GB', { 
      timeZone: 'Europe/London',
      year: 'numeric', 
      month: 'short', 
      day: 'numeric',
      hour: '2-digit', 
      minute: '2-digit'
    })
  }
}