// pages/api/database-status.js - 查看数据库状态和数据的工具

import { WaterQualityDB } from '../../lib/database'

export default async function handler(req, res) {
  try {
    console.log('🔍 Checking database status...')
    
    // 测试数据库连接
    const isConnected = await WaterQualityDB.testConnection()
    
    if (!isConnected) {
      return res.status(500).json({
        success: false,
        message: 'Database connection failed'
      })
    }

    // 初始化数据库（如果需要）
    await WaterQualityDB.initializeDatabase()

    // 获取所有数据
    const allData = await WaterQualityDB.getRecentReadings('water-monitor', 100)
    
    // 获取统计信息
    const stats = await WaterQualityDB.getStatistics('water-monitor', 30)
    
    // 获取数据库状态
    const dbStatus = await WaterQualityDB.getDatabaseStatus()

    // 格式化数据用于显示
    const formattedData = allData.map(record => ({
      id: record.id,
      temperature: record.temperature,
      ph: record.ph,
      turbidity: record.turbidity,
      conductivity: record.conductivity,
      tds: record.tds,
      status: record.status,
      recorded_at: record.recorded_at,
      created_at: record.created_at,
      raw_data: record.raw_data
    }))

    return res.status(200).json({
      success: true,
      database_connected: isConnected,
      total_records: allData.length,
      latest_record: allData[0] || null,
      oldest_record: allData[allData.length - 1] || null,
      statistics: stats,
      database_status: dbStatus,
      recent_data: formattedData.slice(0, 10), // 最近10条记录
      all_data: formattedData // 所有数据
    })

  } catch (error) {
    console.error('❌ Database status check error:', error)
    return res.status(500).json({
      success: false,
      message: error.message,
      error: error.toString()
    })
  }
}