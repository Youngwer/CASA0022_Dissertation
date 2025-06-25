// pages/api/water-data.js - 修复导入问题的版本

// 修复导入方式
import WaterQualityDB from '../../lib/database'
// 或者使用命名导入
// import { WaterQualityDB } from '../../lib/database'

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
      // 数据库中没有数据，尝试从TTN获取 (作为后备)
      console.log('⚠️ No data in database, trying TTN as fallback...')
      
      const ttnData = await tryGetTTNData()
      if (ttnData) {
        // 保存TTN数据到数据库
        try {
          await WaterQualityDB.saveReading({
            device_id: DEVICE_ID,
            ...ttnData,
            recorded_at: new Date(),
            raw_data: { source: 'ttn_fallback' }
          })
          console.log('💾 TTN fallback data saved to database')
        } catch (saveError) {
          console.error('❌ Failed to save TTN fallback data:', saveError.message)
        }
        
        return res.status(200).json({
          success: true,
          data: ttnData,
          source: 'ttn_fallback',
          message: 'Data from TTN (saved to database)',
          timestamp: new Date().toISOString()
        })
      } else {
        // 完全没有数据，返回模拟数据
        console.log('⚠️ No data available, using mock data')
        return res.status(200).json({
          success: true,
          data: getMockData(),
          source: 'mock',
          message: 'No real data available',
          timestamp: new Date().toISOString()
        })
      }
    }

  } catch (error) {
    console.error('💥 API Error:', error.message)
    console.error('💥 Error stack:', error.stack)
    
    return res.status(200).json({
      success: true,
      data: getMockData(),
      source: 'mock',
      message: `System error: ${error.message}`,
      timestamp: new Date().toISOString()
    })
  }
}

// 尝试从TTN获取数据 (后备方案)
async function tryGetTTNData() {
  try {
    const TTN_APP_ID = process.env.TTN_APP_ID
    const TTN_API_KEY = process.env.TTN_API_KEY
    const TTN_REGION = process.env.TTN_REGION || 'eu1'
    const DEVICE_ID = process.env.DEVICE_ID

    if (!TTN_APP_ID || !TTN_API_KEY || !DEVICE_ID) {
      return null
    }

    const storageUrl = `https://${TTN_REGION}.cloud.thethings.network/api/v3/as/applications/${TTN_APP_ID}/devices/${DEVICE_ID}/packages/storage/uplink_message?limit=1&order=-received_at`

    const response = await fetch(storageUrl, {
      method: 'GET',
      headers: {
        'Authorization': `Bearer ${TTN_API_KEY}`,
        'Accept': 'application/json',
      },
      timeout: 5000
    })

    if (response.ok) {
      const responseText = await response.text()
      if (responseText.trim().length > 0) {
        const ttnData = JSON.parse(responseText)
        return processTTNStorageData(ttnData)
      }
    }
    
    return null
  } catch (error) {
    console.error('TTN fallback failed:', error.message)
    return null
  }
}

// 处理TTN数据
function processTTNStorageData(ttnData) {
  try {
    const payload = ttnData.result?.uplink_message?.decoded_payload
    const receivedAt = ttnData.result?.received_at
    
    if (!payload) return null

    const safeParseFloat = (value, defaultValue = 0) => {
      const parsed = parseFloat(value)
      return isNaN(parsed) ? defaultValue : parsed
    }

    const waterData = {
      temperature: safeParseFloat(payload.temperature, 0),
      ph: safeParseFloat(payload.ph, 7),
      turbidity: safeParseFloat(payload.turbidity, 0),
      conductivity: safeParseFloat(payload.conductivity, 0),
      tds: safeParseFloat(payload.tds, 0),
      status: mapTTNStatusToFrontend(payload.status),
      lastUpdate: receivedAt ? 
        new Date(receivedAt).toLocaleString('en-GB', { 
          timeZone: 'Europe/London',
          year: 'numeric', 
          month: 'short', 
          day: 'numeric',
          hour: '2-digit', 
          minute: '2-digit'
        }) : 
        'Unknown'
    }

    return waterData
  } catch (error) {
    console.error('Error processing TTN data:', error)
    return null
  }
}

// TTN状态映射到前端状态
function mapTTNStatusToFrontend(ttnStatus) {
  const statusMap = {
    'excellent': 'EXCELLENT',
    'marginal': 'MARGINAL',
    'unsafe': 'UNSAFE'
  }
  return statusMap[ttnStatus] || 'UNKNOWN'
}

// 格式化数据库数据
function formatDatabaseData(dbData) {
  if (!dbData) return getMockData()
  
  const safeParseFloat = (value, defaultValue = 0) => {
    if (value === null || value === undefined) return defaultValue
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

// 模拟数据
function getMockData() {
  return {
    temperature: 22.5,
    ph: 7.2,
    turbidity: 0.8,
    conductivity: 350.0,
    tds: 280.0,
    status: 'EXCELLENT',
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