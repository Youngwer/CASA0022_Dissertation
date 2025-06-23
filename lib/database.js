// lib/database.js - 数据库操作工具
import { Pool } from 'pg'

// 创建数据库连接池
const pool = new Pool({
  connectionString: process.env.DATABASE_URL,
  ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
  max: 10,
  idleTimeoutMillis: 30000,
  connectionTimeoutMillis: 2000,
})

// 数据库操作类
export class WaterQualityDB {
  
  // 初始化数据库表（首次运行时使用）
  static async initializeDatabase() {
    const client = await pool.connect()
    
    try {
      // 创建水质读数表
      await client.query(`
        CREATE TABLE IF NOT EXISTS water_quality_readings (
          id SERIAL PRIMARY KEY,
          device_id VARCHAR(100) NOT NULL,
          temperature DECIMAL(5,2),
          ph DECIMAL(4,2),
          turbidity DECIMAL(8,1),
          conductivity INTEGER,
          tds INTEGER,
          status VARCHAR(20),
          raw_data JSONB,
          recorded_at TIMESTAMP NOT NULL,
          created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
      `)

      // 创建索引提高查询性能
      await client.query(`
        CREATE INDEX IF NOT EXISTS idx_device_recorded_at 
        ON water_quality_readings (device_id, recorded_at)
      `)

      await client.query(`
        CREATE INDEX IF NOT EXISTS idx_recorded_at 
        ON water_quality_readings (recorded_at DESC)
      `)

      console.log('✅ Database tables initialized successfully')
      return true
      
    } catch (error) {
      console.error('❌ Error initializing database:', error)
      return false
    } finally {
      client.release()
    }
  }

  // 保存水质读数到数据库
  static async saveReading(data) {
    const client = await pool.connect()
    
    try {
      const query = `
        INSERT INTO water_quality_readings 
        (device_id, temperature, ph, turbidity, conductivity, tds, status, raw_data, recorded_at)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)
        RETURNING id, created_at
      `
      
      const values = [
        data.device_id || 'water-monitor',
        data.temperature,
        data.ph,
        data.turbidity,
        data.conductivity,
        data.tds,
        data.status,
        JSON.stringify(data.raw_data || {}),
        data.recorded_at || new Date()
      ]
      
      const result = await client.query(query, values)
      console.log('✅ Data saved to database:', result.rows[0])
      
      return result.rows[0]
      
    } catch (error) {
      console.error('❌ Error saving to database:', error)
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
      console.error('❌ Error getting latest reading:', error)
      return null
    } finally {
      client.release()
    }
  }

  // 获取历史数据（指定时间范围）
  static async getHistoricalData(deviceId = 'water-monitor', hours = 24) {
    const client = await pool.connect()
    
    try {
      const query = `
        SELECT * FROM water_quality_readings 
        WHERE device_id = $1 
        AND recorded_at >= NOW() - INTERVAL '${hours} hours'
        ORDER BY recorded_at ASC
      `
      
      const result = await client.query(query, [deviceId])
      return result.rows
      
    } catch (error) {
      console.error('❌ Error getting historical data:', error)
      return []
    } finally {
      client.release()
    }
  }

  // 获取统计信息
  static async getStatistics(deviceId = 'water-monitor', days = 30) {
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
          COUNT(CASE WHEN status = 'GOOD' THEN 1 END) as good_readings,
          COUNT(CASE WHEN status = 'OK' THEN 1 END) as ok_readings,
          COUNT(CASE WHEN status = 'POOR' THEN 1 END) as poor_readings
        FROM water_quality_readings 
        WHERE device_id = $1 
        AND recorded_at >= NOW() - INTERVAL '${days} days'
      `
      
      const result = await client.query(query, [deviceId])
      return result.rows[0]
      
    } catch (error) {
      console.error('❌ Error getting statistics:', error)
      return null
    } finally {
      client.release()
    }
  }

  // 测试数据库连接
  static async testConnection() {
    try {
      const client = await pool.connect()
      const result = await client.query('SELECT NOW() as current_time')
      client.release()
      console.log('✅ Database connection successful:', result.rows[0])
      return true
    } catch (error) {
      console.error('❌ Database connection failed:', error)
      return false
    }
  }
}

export default WaterQualityDB