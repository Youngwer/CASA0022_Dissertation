// pages/api/migrate-database.js - 数据库迁移API（添加水质标签字段）

import { Pool } from 'pg'

const pool = new Pool({
  connectionString: process.env.DATABASE_URL,
  ssl: process.env.NODE_ENV === 'production' ? { rejectUnauthorized: false } : false,
})

export default async function handler(req, res) {
  if (req.method !== 'POST') {
    return res.status(405).json({ message: 'Method not allowed' })
  }

  const client = await pool.connect()
  
  try {
    console.log('🔄 Starting database migration for water labels...')

    // 检查是否需要添加水质标签列
    const columnCheck = await client.query(`
      SELECT column_name 
      FROM information_schema.columns 
      WHERE table_name = 'water_quality_readings' 
      AND column_name IN ('water_label', 'updated_at')
    `)

    const existingColumns = columnCheck.rows.map(row => row.column_name)
    const needsWaterLabel = !existingColumns.includes('water_label')
    const needsUpdatedAt = !existingColumns.includes('updated_at')

    const migrations = []

    if (needsWaterLabel) {
      console.log('📝 Adding water_label column...')
      await client.query(`
        ALTER TABLE water_quality_readings 
        ADD COLUMN water_label VARCHAR(100)
      `)
      migrations.push('Added water_label column')
    }

    if (needsUpdatedAt) {
      console.log('📝 Adding updated_at column...')
      await client.query(`
        ALTER TABLE water_quality_readings 
        ADD COLUMN updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      `)
      migrations.push('Added updated_at column')
    }

    // 添加水质标签索引（如果不存在）
    try {
      await client.query(`
        CREATE INDEX IF NOT EXISTS idx_water_label 
        ON water_quality_readings (water_label)
      `)
      migrations.push('Added water_label index')
    } catch (indexError) {
      console.log('ℹ️ Index already exists or creation failed:', indexError.message)
    }

    // 检查表结构
    const tableStructure = await client.query(`
      SELECT column_name, data_type, is_nullable, column_default
      FROM information_schema.columns 
      WHERE table_name = 'water_quality_readings'
      ORDER BY ordinal_position
    `)

    console.log('✅ Database migration completed')

    return res.status(200).json({
      success: true,
      message: 'Database migration completed successfully',
      migrations_applied: migrations,
      table_structure: tableStructure.rows
    })

  } catch (error) {
    console.error('❌ Database migration error:', error)
    return res.status(500).json({
      success: false,
      message: 'Database migration failed',
      error: error.message
    })
  } finally {
    client.release()
  }
}