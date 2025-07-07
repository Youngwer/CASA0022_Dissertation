// pages/api/recent-history.js - 支持水质标签的历史记录API

import WaterQualityDB from '../../lib/database'

export default async function handler(req, res) {
  if (req.method !== 'GET') {
    return res.status(405).json({ message: 'Method not allowed' })
  }

  try {
    const { limit = 20 } = req.query
    const limitNum = parseInt(limit)

    console.log(`📊 Fetching recent ${limitNum} records with water labels`)

    // 初始化数据库连接
    await WaterQualityDB.initializeDatabase()
    const dbConnected = await WaterQualityDB.testConnection()

    if (!dbConnected) {
      console.log('❌ Database not connected, returning mock data')
      return res.status(200).json({
        success: true,
        data: getMockHistoryData(),
        source: 'mock',
        message: 'Database connection failed, showing demo data'
      })
    }

    const DEVICE_ID = process.env.DEVICE_ID || 'water-monitor'
    
    // 获取最近的记录（包含水质标签）
    const recentRecords = await WaterQualityDB.getRecentReadings(DEVICE_ID, limitNum)
    
    if (recentRecords && recentRecords.length > 0) {
      // 格式化数据
      const formattedRecords = recentRecords.map(record => ({
        id: record.id,
        temperature: parseFloat(record.temperature) || 0,
        ph: parseFloat(record.ph) || 0,
        turbidity: parseFloat(record.turbidity) || 0,
        conductivity: parseFloat(record.conductivity) || 0,
        tds: parseFloat(record.tds) || 0,
        status: record.status || 'UNKNOWN',
        water_label: record.water_label || null, // 包含水质标签
        recorded_at: record.recorded_at,
        created_at: record.created_at
      }))

      return res.status(200).json({
        success: true,
        data: formattedRecords,
        source: 'database',
        count: formattedRecords.length,
        timestamp: new Date().toISOString()
      })
    } else {
      // 没有数据时返回模拟数据
      return res.status(200).json({
        success: true,
        data: getMockHistoryData(),
        source: 'mock',
        message: 'No records found in database, showing demo data'
      })
    }

  } catch (error) {
    console.error('❌ Recent history API error:', error)
    
    // 发生错误时返回模拟数据
    return res.status(200).json({
      success: true,
      data: getMockHistoryData(),
      source: 'mock',
      message: `Database error: ${error.message}`,
      error: error.toString()
    })
  }
}

// 模拟历史数据（包含水质标签）
function getMockHistoryData() {
  const now = new Date()
  const records = []
  
  // 不同类型的水源标签示例
  const waterSources = [
    'Brita Filtered Water',
    'Tap Water',
    'Bottled Water (Evian)',
    'Well Water',
    'Spring Water',
    'RO Filtered Water',
    'Distilled Water',
    'Lake Water',
    'Rainwater',
    'Bottled Water (Aquafina)'
  ]
  
  // 生成最近20条模拟记录
  for (let i = 0; i < 20; i++) {
    const recordTime = new Date(now.getTime() - (i * 2 * 60 * 60 * 1000)) // 每2小时一条记录
    const waterSource = waterSources[i % waterSources.length]
    
    // 根据不同水源生成相应的水质数据
    let ph, temperature, tds, turbidity, conductivity
    
    switch (waterSource) {
      case 'Brita Filtered Water':
        ph = 7.0 + Math.random() * 0.4 // 7.0-7.4
        tds = 180 + Math.random() * 40 // 180-220
        turbidity = 0.2 + Math.random() * 0.6 // 0.2-0.8
        break
      case 'Tap Water':
        ph = 6.8 + Math.random() * 0.6 // 6.8-7.4
        tds = 200 + Math.random() * 100 // 200-300
        turbidity = 0.5 + Math.random() * 1.0 // 0.5-1.5
        break
      case 'Distilled Water':
        ph = 6.5 + Math.random() * 0.5 // 6.5-7.0
        tds = 5 + Math.random() * 15 // 5-20 (很低)
        turbidity = 0.1 + Math.random() * 0.2 // 0.1-0.3
        break
      case 'Well Water':
        ph = 6.5 + Math.random() * 1.0 // 6.5-7.5
        tds = 250 + Math.random() * 150 // 250-400
        turbidity = 0.8 + Math.random() * 1.5 // 0.8-2.3
        break
      default:
        ph = 6.5 + Math.random() * 1.5 // 6.5-8.0
        tds = 150 + Math.random() * 100 // 150-250
        turbidity = Math.random() * 2 // 0-2
    }
    
    temperature = 20 + Math.random() * 5 // 20-25°C
    conductivity = tds * 1.2 // 大概关系
    
    // 根据参数判断状态
    let status = 'EXCELLENT'
    if (ph < 6.5 || ph > 8.0 || turbidity > 1.5 || tds > 300 || tds < 50) {
      status = 'MARGINAL'
    }
    if (ph < 6.0 || ph > 9.0 || turbidity > 4.0 || tds > 500 || tds < 30) {
      status = 'UNSAFE'
    }

    records.push({
      id: i + 1,
      temperature: Math.round(temperature * 10) / 10,
      ph: Math.round(ph * 10) / 10,
      turbidity: Math.round(turbidity * 10) / 10,
      conductivity: Math.round(conductivity),
      tds: Math.round(tds),
      status,
      water_label: waterSource, // 包含水质标签
      recorded_at: recordTime.toISOString(),
      created_at: recordTime.toISOString()
    })
  }
  
  return records
}