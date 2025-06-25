// pages/api/ttn-webhook.js - TTN Webhook接收器

import WaterQualityDB from '../../lib/database'

export default async function handler(req, res) {
  console.log('🎯 TTN Webhook received request')
  console.log('Method:', req.method)
  console.log('Headers:', req.headers)
  console.log('Body:', JSON.stringify(req.body, null, 2))

  // 只接受POST请求
  if (req.method !== 'POST') {
    return res.status(405).json({ 
      success: false, 
      message: 'Method not allowed. Only POST requests are accepted.' 
    })
  }

  try {
    // 检查TTN数据结构
    if (!req.body) {
      console.log('❌ No body received')
      return res.status(400).json({
        success: false,
        message: 'No data received'
      })
    }

    // TTN发送的数据结构
    const data = req.body
    console.log('📦 Processing TTN data...')

    // 提取设备信息
    const deviceId = data.end_device_ids?.device_id || 'water-monitor'
    const receivedAt = data.received_at || new Date().toISOString()

    // 提取解码后的传感器数据
    const payload = data.uplink_message?.decoded_payload

    if (!payload) {
      console.log('❌ No decoded payload found')
      console.log('Available data:', Object.keys(data))
      return res.status(400).json({
        success: false,
        message: 'No decoded payload found',
        received_data: Object.keys(data)
      })
    }

    console.log('✅ Found payload:', payload)

    // 处理水质数据
    const waterData = {
      temperature: parseFloat(payload.temperature) || 0,
      ph: parseFloat(payload.ph) || 0,
      turbidity: parseFloat(payload.turbidity) || 0,
      conductivity: parseFloat(payload.conductivity) || 0,
      tds: parseFloat(payload.tds) || 0
    }

    // 评估水质状态
    const status = evaluateWaterQuality(waterData)
    waterData.status = status

    console.log('📊 Processed water data:', waterData)

    // 保存到Neon数据库
    try {
      await WaterQualityDB.initializeDatabase()
      
      const savedRecord = await WaterQualityDB.saveReading({
        device_id: deviceId,
        temperature: waterData.temperature,
        ph: waterData.ph,
        turbidity: waterData.turbidity,
        conductivity: waterData.conductivity,
        tds: waterData.tds,
        status: waterData.status,
        recorded_at: new Date(receivedAt),
        raw_data: {
          source: 'ttn_webhook',
          original_payload: payload,
          device_info: data.end_device_ids,
          received_at: receivedAt
        }
      })

      console.log('✅ Data saved to Neon database:', savedRecord.id)

      // 返回成功响应给TTN
      return res.status(200).json({
        success: true,
        message: 'Water quality data received and saved successfully',
        data: {
          record_id: savedRecord.id,
          device_id: deviceId,
          status: waterData.status,
          timestamp: receivedAt,
          saved_data: waterData
        }
      })

    } catch (dbError) {
      console.error('❌ Database save error:', dbError)
      return res.status(500).json({
        success: false,
        message: 'Failed to save data to database',
        error: dbError.message
      })
    }

  } catch (error) {
    console.error('💥 Webhook processing error:', error)
    return res.status(500).json({
      success: false,
      message: 'Internal server error',
      error: error.message
    })
  }
}

// 简单的水质评估函数
function evaluateWaterQuality({ ph, turbidity, tds, conductivity }) {
  let issues = 0

  // pH检查 (6.5-8.0 优秀, 6.0-9.0 可接受)
  if (ph < 6.0 || ph > 9.0) issues += 2
  else if (ph < 6.5 || ph > 8.0) issues += 1

  // 浊度检查 (≤1.0 优秀, ≤4.0 可接受)
  if (turbidity > 4.0) issues += 2
  else if (turbidity > 1.0) issues += 1

  // TDS检查 (80-300 优秀, 50-500 可接受)
  if (tds < 50 || tds > 500) issues += 2
  else if (tds < 80 || tds > 300) issues += 1

  // 电导率检查 (100-400 优秀, 50-800 可接受)
  if (conductivity < 50 || conductivity > 800) issues += 2
  else if (conductivity < 100 || conductivity > 400) issues += 1

  if (issues >= 3) return 'UNSAFE'
  else if (issues >= 1) return 'MARGINAL'
  else return 'EXCELLENT'
}