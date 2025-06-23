// pages/api/water-data.js - 简化日志输出版本
import { WaterQualityDB } from '../../lib/database'

export default async function handler(req, res) {
  if (req.method !== 'GET') {
    return res.status(405).json({ message: 'Method not allowed' })
  }

  try {
    console.log('🚀 Fetching water data...')
    
    // 初始化数据库（静默）
    await WaterQualityDB.initializeDatabase()
    const dbConnected = await WaterQualityDB.testConnection()

    // TTN配置
    const TTN_APP_ID = process.env.TTN_APP_ID
    const TTN_API_KEY = process.env.TTN_API_KEY
    const TTN_REGION = process.env.TTN_REGION || 'eu1'
    const DEVICE_ID = process.env.DEVICE_ID

    // 检查TTN配置
    if (!TTN_APP_ID || !TTN_API_KEY || !DEVICE_ID) {
      console.log('⚠️ TTN config missing, using demo data')
      return res.status(200).json({
        success: true,
        data: getMockData(),
        source: 'mock',
        message: 'TTN configuration incomplete',
        timestamp: new Date().toISOString()
      })
    }

    let waterData = null
    let dataSource = 'unknown'
    let message = ''

    // 检查数据库中的最新数据
    const latestFromDB = await WaterQualityDB.getLatestReading(DEVICE_ID)
    const isRecentData = latestFromDB && 
      (new Date() - new Date(latestFromDB.recorded_at)) < 10 * 60 * 1000

    if (isRecentData) {
      waterData = formatDatabaseData(latestFromDB)
      dataSource = 'database'
      message = 'Recent database data'
      console.log('📊 Using recent database data')
    } else {
      // 获取TTN新数据
      try {
        console.log('📡 Querying TTN...')
        
        const storageUrl = `https://${TTN_REGION}.cloud.thethings.network/api/v3/as/applications/${TTN_APP_ID}/devices/${DEVICE_ID}/packages/storage/uplink_message?limit=1&order=-received_at`

        const response = await fetch(storageUrl, {
          method: 'GET',
          headers: {
            'Authorization': `Bearer ${TTN_API_KEY}`,
            'Accept': 'application/json',
          },
          timeout: 10000
        })

        if (response.ok) {
          const responseText = await response.text()
          
          if (responseText.trim().length === 0) {
            console.log('⚠️ TTN empty response')
            message = 'No data in TTN storage'
            dataSource = 'mock'
            waterData = getMockData()
          } else {
            const ttnData = JSON.parse(responseText)
            const processedData = processTTNStorageData(ttnData)
            
            if (processedData) {
              waterData = processedData
              dataSource = 'ttn_storage'
              message = 'Fresh TTN data'
              console.log('✅ TTN data processed successfully')
              
              // 保存到数据库
              if (dbConnected) {
                await WaterQualityDB.saveReading({
                  device_id: DEVICE_ID,
                  ...processedData,
                  recorded_at: new Date()
                })
                console.log('💾 Saved to database')
              }
            } else {
              throw new Error('Failed to process TTN data')
            }
          }
        } else {
          throw new Error(`TTN API error: ${response.status}`)
        }

      } catch (ttnError) {
        console.log('❌ TTN error:', ttnError.message)
        
        // 回退到数据库或模拟数据
        if (latestFromDB) {
          waterData = formatDatabaseData(latestFromDB)
          dataSource = 'database_fallback'
          message = `TTN unavailable, using DB data`
        } else {
          waterData = getMockData()
          dataSource = 'mock'
          message = 'TTN unavailable, using demo data'
        }
      }
    }

    console.log(`✅ Response: ${dataSource} | ${waterData?.temperature}°C, pH ${waterData?.ph}`)
    
    return res.status(200).json({
      success: true,
      data: waterData,
      source: dataSource,
      message: message,
      timestamp: new Date().toISOString()
    })

  } catch (error) {
    console.error('💥 API Error:', error.message)
    
    return res.status(200).json({
      success: true,
      data: getMockData(),
      source: 'mock',
      message: `System error: ${error.message}`,
      timestamp: new Date().toISOString()
    })
  }
}

// 处理TTN数据（简化日志）
function processTTNStorageData(ttnData) {
  try {
    let payload = null
    let receivedAt = null
    
    // 提取payload
    if (ttnData.result && ttnData.result.uplink_message && ttnData.result.uplink_message.decoded_payload) {
      payload = ttnData.result.uplink_message.decoded_payload
      receivedAt = ttnData.result.received_at || ttnData.result.uplink_message.received_at
    }

    if (!payload) {
      console.log('❌ No payload found')
      return null
    }

    // 转换数据格式
    const waterData = {
      temperature: payload.temperature || 0,
      ph: payload.ph || 0,
      turbidity: payload.turbidity || 0,
      conductivity: payload.conductivity || 0,
      tds: payload.tds || 0,
      lastUpdate: receivedAt ? new Date(receivedAt).toLocaleString() : new Date().toLocaleString(),
      receivedAt: receivedAt,
      raw_data: payload
    }

    // 使用payload中的状态或自己计算
    waterData.status = payload.status || calculateWaterQualityStatus(waterData)

    return waterData

  } catch (error) {
    console.error('❌ Process error:', error.message)
    return null
  }
}

// 格式化数据库数据
function formatDatabaseData(dbData) {
  return {
    temperature: parseFloat(dbData.temperature) || 0,
    ph: parseFloat(dbData.ph) || 0,
    turbidity: parseFloat(dbData.turbidity) || 0,
    conductivity: parseInt(dbData.conductivity) || 0,
    tds: parseInt(dbData.tds) || 0,
    status: dbData.status || 'UNKNOWN',
    lastUpdate: new Date(dbData.recorded_at).toLocaleString(),
    rawData: dbData.raw_data
  }
}

// 计算水质状态
function calculateWaterQualityStatus(data) {
  const { ph, turbidity, tds } = data
  
  const phGood = (ph >= 6.5 && ph <= 8.5)
  const turbidityGood = (turbidity < 4.0)
  const tdsGood = (tds < 500)
  
  if (phGood && turbidityGood && tdsGood) {
    return 'GOOD'
  } else if (ph >= 6.0 && ph <= 9.0 && turbidity < 10.0 && tds < 1000) {
    return 'OK'
  } else {
    return 'POOR'
  }
}

// 模拟数据
function getMockData() {
  return {
    temperature: 23.12,
    ph: 7.06,
    turbidity: 0.8,
    conductivity: 5,
    tds: 2,
    status: 'GOOD',
    lastUpdate: new Date().toLocaleString(),
    source: 'demo'
  }
}