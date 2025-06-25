// pages/database.js - 数据库数据查看页面

import { useState, useEffect } from 'react'
import Head from 'next/head'

export default function DatabaseViewer() {
  const [data, setData] = useState(null)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState(null)

  const fetchDatabaseStatus = async () => {
    try {
      setLoading(true)
      setError(null)
      
      const response = await fetch('/api/database-status')
      const result = await response.json()
      
      if (result.success) {
        setData(result)
      } else {
        setError(result.message)
      }
    } catch (err) {
      setError(err.message)
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    fetchDatabaseStatus()
  }, [])

  if (loading) {
    return (
      <div style={{ padding: '2rem', textAlign: 'center' }}>
        <h1>正在检查数据库...</h1>
        <p>Loading...</p>
      </div>
    )
  }

  if (error) {
    return (
      <div style={{ padding: '2rem', textAlign: 'center', color: 'red' }}>
        <h1>数据库连接错误</h1>
        <p>{error}</p>
        <button onClick={fetchDatabaseStatus} style={{ padding: '0.5rem 1rem', marginTop: '1rem' }}>
          重试
        </button>
      </div>
    )
  }

  return (
    <div style={{ padding: '2rem', fontFamily: 'Arial, sans-serif' }}>
      <Head>
        <title>数据库状态查看器</title>
      </Head>

      <h1>🗄️ Neon数据库状态</h1>
      
      <button 
        onClick={fetchDatabaseStatus} 
        style={{ 
          padding: '0.5rem 1rem', 
          marginBottom: '2rem',
          backgroundColor: '#007bff',
          color: 'white',
          border: 'none',
          borderRadius: '4px',
          cursor: 'pointer'
        }}
      >
        🔄 刷新数据
      </button>

      {/* 数据库连接状态 */}
      <div style={{ 
        backgroundColor: data?.database_connected ? '#d4edda' : '#f8d7da', 
        padding: '1rem', 
        borderRadius: '4px', 
        marginBottom: '2rem' 
      }}>
        <h2>📡 连接状态</h2>
        <p><strong>数据库连接:</strong> {data?.database_connected ? '✅ 成功' : '❌ 失败'}</p>
        <p><strong>总记录数:</strong> {data?.total_records || 0}</p>
      </div>

      {/* 统计信息 */}
      {data?.statistics && (
        <div style={{ backgroundColor: '#f8f9fa', padding: '1rem', borderRadius: '4px', marginBottom: '2rem' }}>
          <h2>📊 统计信息 (最近30天)</h2>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: '1rem' }}>
            <div>
              <p><strong>总读数:</strong> {data.statistics.total_readings}</p>
              <p><strong>平均温度:</strong> {data.statistics.avg_temperature ? parseFloat(data.statistics.avg_temperature).toFixed(1) + '°C' : 'N/A'}</p>
              <p><strong>平均pH:</strong> {data.statistics.avg_ph ? parseFloat(data.statistics.avg_ph).toFixed(2) : 'N/A'}</p>
            </div>
            <div>
              <p><strong>优秀数据:</strong> {data.statistics.excellent_readings || 0}</p>
              <p><strong>一般数据:</strong> {data.statistics.marginal_readings || 0}</p>
              <p><strong>不安全数据:</strong> {data.statistics.unsafe_readings || 0}</p>
            </div>
          </div>
        </div>
      )}

      {/* 最新记录 */}
      {data?.latest_record && (
        <div style={{ backgroundColor: '#e7f3ff', padding: '1rem', borderRadius: '4px', marginBottom: '2rem' }}>
          <h2>🕐 最新记录</h2>
          <p><strong>记录时间:</strong> {new Date(data.latest_record.recorded_at).toLocaleString()}</p>
          <p><strong>温度:</strong> {data.latest_record.temperature}°C</p>
          <p><strong>pH:</strong> {data.latest_record.ph}</p>
          <p><strong>浊度:</strong> {data.latest_record.turbidity} NTU</p>
          <p><strong>电导率:</strong> {data.latest_record.conductivity} μS/cm</p>
          <p><strong>TDS:</strong> {data.latest_record.tds} ppm</p>
          <p><strong>状态:</strong> {data.latest_record.status}</p>
        </div>
      )}

      {/* 最近数据列表 */}
      {data?.recent_data && data.recent_data.length > 0 && (
        <div style={{ marginBottom: '2rem' }}>
          <h2>📋 最近10条记录</h2>
          <div style={{ overflow: 'auto' }}>
            <table style={{ width: '100%', borderCollapse: 'collapse', fontSize: '0.9rem' }}>
              <thead>
                <tr style={{ backgroundColor: '#f8f9fa' }}>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>ID</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>时间</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>温度</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>pH</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>浊度</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>电导率</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>TDS</th>
                  <th style={{ border: '1px solid #ddd', padding: '0.5rem' }}>状态</th>
                </tr>
              </thead>
              <tbody>
                {data.recent_data.map((record) => (
                  <tr key={record.id}>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.id}</td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>
                      {new Date(record.recorded_at).toLocaleString()}
                    </td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.temperature}°C</td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.ph}</td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.turbidity}</td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.conductivity}</td>
                    <td style={{ border: '1px solid #ddd', padding: '0.5rem' }}>{record.tds}</td>
                    <td style={{ 
                      border: '1px solid #ddd', 
                      padding: '0.5rem',
                      backgroundColor: record.status === 'EXCELLENT' ? '#d4edda' : 
                                     record.status === 'MARGINAL' ? '#fff3cd' : 
                                     record.status === 'UNSAFE' ? '#f8d7da' : '#f8f9fa'
                    }}>
                      {record.status}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* 无数据提示 */}
      {(!data?.recent_data || data.recent_data.length === 0) && (
        <div style={{ 
          backgroundColor: '#fff3cd', 
          padding: '2rem', 
          textAlign: 'center', 
          borderRadius: '4px',
          marginBottom: '2rem'
        }}>
          <h2>📭 数据库中暂无数据</h2>
          <p>这说明TTN数据还没有保存到您的Neon数据库中。</p>
          <p>可能的原因：</p>
          <ul style={{ textAlign: 'left', maxWidth: '500px', margin: '0 auto' }}>
            <li>TTN Webhook还没有配置</li>
            <li>Arduino还没有发送数据</li>
            <li>数据还在TTN Storage中，没有保存到数据库</li>
          </ul>
        </div>
      )}

      {/* 原始数据 (调试用) */}
      <details style={{ marginTop: '2rem' }}>
        <summary style={{ cursor: 'pointer', padding: '0.5rem', backgroundColor: '#f8f9fa' }}>
          🔧 原始数据 (调试用)
        </summary>
        <pre style={{ 
          backgroundColor: '#f8f9fa', 
          padding: '1rem', 
          overflow: 'auto',
          fontSize: '0.8rem',
          borderRadius: '4px'
        }}>
          {JSON.stringify(data, null, 2)}
        </pre>
      </details>
    </div>
  )
}