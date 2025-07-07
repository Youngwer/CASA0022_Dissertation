// pages/api/update-water-label.js - 修复后的水质标签更新API

import WaterQualityDB from '../../lib/database'

export default async function handler(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ message: 'Method not allowed' })
  }

  try {
    const { label } = req.body

    if (!label || typeof label !== 'string' || label.trim().length === 0) {
      return res.status(400).json({
        success: false,
        message: 'Water label is required and must be a non-empty string'
      })
    }

    const trimmedLabel = label.trim()
    
    if (trimmedLabel.length > 50) {
      return res.status(400).json({
        success: false,
        message: 'Water label must be 50 characters or less'
      })
    }

    console.log(`🏷️ Updating water label: "${trimmedLabel}"`)

    // 初始化数据库连接
    await WaterQualityDB.initializeDatabase()
    const dbConnected = await WaterQualityDB.testConnection()

    if (!dbConnected) {
      return res.status(500).json({
        success: false,
        message: 'Database connection failed'
      })
    }

    const DEVICE_ID = process.env.DEVICE_ID || 'water-monitor'

    // 获取最新的记录
    const latestRecord = await WaterQualityDB.getLatestReading(DEVICE_ID)
    
    if (!latestRecord) {
      return res.status(404).json({
        success: false,
        message: 'No recent measurement found to label'
      })
    }

    // 直接使用数据库连接池来执行更新操作
    const { Pool } = require('pg')
    const pool = new Pool({
      connectionString: process.env.DATABASE_URL,
      ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
    })

    const client = await pool.connect()
    
    try {
      // 首先确保 water_label 列存在
      await client.query(`
        ALTER TABLE water_quality_readings 
        ADD COLUMN IF NOT EXISTS water_label VARCHAR(100)
      `)

      // 更新最新记录的水质标签
      const updateResult = await client.query(`
        UPDATE water_quality_readings 
        SET water_label = $1
        WHERE id = $2
        RETURNING *
      `, [trimmedLabel, latestRecord.id])

      if (updateResult.rows.length === 0) {
        throw new Error('Failed to update water label')
      }

      const updatedRecord = updateResult.rows[0]

      console.log(`✅ Water label updated successfully for record ID: ${updatedRecord.id}`)

      return res.status(200).json({
        success: true,
        message: 'Water label updated successfully',
        data: {
          record_id: updatedRecord.id,
          water_label: updatedRecord.water_label,
          updated_at: new Date().toISOString()
        }
      })

    } finally {
      client.release()
      await pool.end()
    }

  } catch (error) {
    console.error('❌ Water label update error:', error)
    return res.status(500).json({
      success: false,
      message: 'Failed to update water label',
      error: error.message
    })
  }
}