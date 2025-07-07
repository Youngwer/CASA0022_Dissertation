// lib/database.js - 修复导出问题的版本

import { Pool } from 'pg'

// 创建数据库连接池 (使用您的Neon数据库)
const pool = new Pool({
  connectionString: process.env.DATABASE_URL,
  ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
  max: 20,
  idleTimeoutMillis: 30000,
  connectionTimeoutMillis: 5000,
  application_name: 'water-quality-monitor'
})

// 数据库操作类
export class WaterQualityDB {
  
  // 初始化数据库表
// lib/database.js - 更新数据库表结构以支持水质标签

// 在 WaterQualityDB.initializeDatabase() 方法中更新表结构：

static async initializeDatabase() {
  const client = await pool.connect()
  
  try {
    // 创建水质读数表 (添加水质标签字段)
    await client.query(`
      CREATE TABLE IF NOT EXISTS water_quality_readings (
        id SERIAL PRIMARY KEY,
        device_id VARCHAR(100) NOT NULL,
        temperature DECIMAL(5,2),
        ph DECIMAL(4,2),
        turbidity DECIMAL(8,2),
        conductivity DECIMAL(8,2),
        tds DECIMAL(8,2),
        status VARCHAR(20),
        water_label VARCHAR(100), -- 新增：水质标签字段
        raw_data JSONB,
        recorded_at TIMESTAMP NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      )
    `)

    // 检查是否需要添加水质标签列（用于现有表的迁移）
    const columnExists = await client.query(`
      SELECT column_name 
      FROM information_schema.columns 
      WHERE table_name='water_quality_readings' AND column_name='water_label'
    `)

    if (columnExists.rows.length === 0) {
      console.log('🔄 Adding water_label column to existing table...')
      await client.query(`
        ALTER TABLE water_quality_readings 
        ADD COLUMN water_label VARCHAR(100),
        ADD COLUMN updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      `)
      console.log('✅ Water label column added successfully')
    }

    // 创建优化的索引
    await client.query(`
      CREATE INDEX IF NOT EXISTS idx_device_recorded_at 
      ON water_quality_readings (device_id, recorded_at DESC)
    `)

    await client.query(`
      CREATE INDEX IF NOT EXISTS idx_recorded_at 
      ON water_quality_readings (recorded_at DESC)
    `)

    await client.query(`
      CREATE INDEX IF NOT EXISTS idx_status_created_at 
      ON water_quality_readings (status, created_at DESC)
    `)

    // 添加水质标签索引
    await client.query(`
      CREATE INDEX IF NOT EXISTS idx_water_label 
      ON water_quality_readings (water_label)
    `)

    console.log('✅ Neon database tables initialized successfully with water label support')
    return true
    
  } catch (error) {
    console.error('❌ Error initializing Neon database:', error)
    return false
  } finally {
    client.release()
  }
}

// 更新保存读数方法以包含水质标签
static async saveReading(data) {
  const client = await pool.connect()
  
  try {
    // 数据验证和类型转换
    const processedData = {
      device_id: data.device_id || 'water-monitor',
      temperature: parseFloat(data.temperature) || 0,
      ph: parseFloat(data.ph) || 0,
      turbidity: parseFloat(data.turbidity) || 0,
      conductivity: parseFloat(data.conductivity) || 0,
      tds: parseFloat(data.tds) || 0,
      status: data.status || 'UNKNOWN',
      water_label: data.water_label || null, // 新增：水质标签
      raw_data: data.raw_data || {},
      recorded_at: data.recorded_at || new Date()
    }

    console.log('💾 Saving to Neon database:', processedData)

    const query = `
      INSERT INTO water_quality_readings (
        device_id, temperature, ph, turbidity, conductivity, tds, 
        status, water_label, raw_data, recorded_at
      ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)
      RETURNING *
    `

    const values = [
      processedData.device_id,
      processedData.temperature,
      processedData.ph,
      processedData.turbidity,
      processedData.conductivity,
      processedData.tds,
      processedData.status,
      processedData.water_label,
      JSON.stringify(processedData.raw_data),
      processedData.recorded_at
    ]

    const result = await client.query(query, values)
    const savedRecord = result.rows[0]

    console.log('✅ Data saved to Neon database with ID:', savedRecord.id)
    return savedRecord

  } catch (error) {
    console.error('❌ Error saving to Neon database:', error)
    console.error('Failed data:', data)
    throw error
  } finally {
    client.release()
  }
}

  // 获取最新的水质读数
  static async getLatestReading(deviceId = 'water-monitor') {
    const client = await pool.connect()
    
    try {
      const query = `
        SELECT * FROM water_quality_readings 
        WHERE device_id = $1 
        ORDER BY recorded_at DESC 
        LIMIT 1
      `
      
      const result = await client.query(query, [deviceId])
      return result.rows[0] || null
      
    } catch (error) {
      console.error('❌ Error getting latest reading from Neon:', error)
      return null
    } finally {
      client.release()
    }
  }

  // 获取最近N条记录
  static async getRecentReadings(deviceId = 'water-monitor', limit = 50) {
    const client = await pool.connect()
    
    try {
      const query = `
        SELECT * FROM water_quality_readings 
        WHERE device_id = $1 
        ORDER BY recorded_at DESC 
        LIMIT $2
      `
      
      const result = await client.query(query, [deviceId, limit])
      return result.rows
      
    } catch (error) {
      console.error('❌ Error getting recent readings from Neon:', error)
      return []
    } finally {
      client.release()
    }
  }

  // 获取统计信息
  static async getStatistics(deviceId = 'water-monitor', days = 7) {
    const client = await pool.connect()
    
    try {
      const query = `
        SELECT 
          COUNT(*) as total_readings,
          MIN(recorded_at) as first_reading,
          MAX(recorded_at) as last_reading,
          AVG(temperature) as avg_temperature,
          AVG(ph) as avg_ph,
          AVG(turbidity) as avg_turbidity,
          AVG(conductivity) as avg_conductivity,
          AVG(tds) as avg_tds,
          COUNT(CASE WHEN status = 'EXCELLENT' THEN 1 END) as excellent_readings,
          COUNT(CASE WHEN status = 'MARGINAL' THEN 1 END) as marginal_readings,
          COUNT(CASE WHEN status = 'UNSAFE' THEN 1 END) as unsafe_readings
        FROM water_quality_readings 
        WHERE device_id = $1 
        AND recorded_at >= NOW() - INTERVAL '${days} days'
      `
      
      const result = await client.query(query, [deviceId])
      return result.rows[0]
      
    } catch (error) {
      console.error('❌ Error getting statistics from Neon:', error)
      return null
    } finally {
      client.release()
    }
  }

  // 测试数据库连接
  static async testConnection() {
    try {
      const client = await pool.connect()
      const result = await client.query('SELECT NOW() as current_time, version() as pg_version')
      client.release()
      console.log('✅ Neon database connection successful:', {
        time: result.rows[0].current_time,
        version: result.rows[0].pg_version.split(' ')[0] + ' ' + result.rows[0].pg_version.split(' ')[1]
      })
      return true
    } catch (error) {
      console.error('❌ Neon database connection failed:', error)
      return false
    }
  }

  // 获取数据库状态
  static async getDatabaseStatus() {
    const client = await pool.connect()
    
    try {
      const result = await client.query(`
        SELECT 
          schemaname,
          tablename,
          n_tup_ins as inserts,
          n_tup_upd as updates,
          n_tup_del as deletes,
          n_live_tup as live_tuples
        FROM pg_stat_user_tables 
        WHERE tablename = 'water_quality_readings'
      `)
      
      return result.rows[0] || null
      
    } catch (error) {
      console.error('❌ Error getting database status:', error)
      return null
    } finally {
      client.release()
    }
  }
}

// 导出方式：既支持命名导出，也支持默认导出
export default WaterQualityDB