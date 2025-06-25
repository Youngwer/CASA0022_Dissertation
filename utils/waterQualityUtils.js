// utils/waterQualityUtils.js - 更新的水质评价工具函数

// 水质状态颜色函数 - 支持三级评价
export const getStatusColor = (status) => {
  switch(status) {
    case 'EXCELLENT': return '#10B981' // 🟢 绿色 - 优秀
    case 'MARGINAL': return '#F59E0B'  // 🟡 黄色 - 一般
    case 'UNSAFE': return '#EF4444'    // 🔴 红色 - 不安全
    default: return '#6B7280'          // 灰色 - 未知
  }
}

// 水质状态文本函数 - 支持三级评价
export const getStatusText = (status) => {
  switch(status) {
    case 'EXCELLENT': return '🟢 Excellent' // 优秀，适合饮用
    case 'MARGINAL': return '🟡 Marginal'   // 一般，勉强可接受
    case 'UNSAFE': return '🔴 Unsafe'       // 不安全，不适合饮用
    default: return '⚪ Unknown'            // 未知状态
  }
}

// 状态描述函数
export const getStatusDescription = (status) => {
  switch(status) {
    case 'EXCELLENT': 
      return 'Water quality is excellent and safe for drinking. All parameters are within ideal ranges.'
    case 'MARGINAL': 
      return 'Water quality is acceptable but not ideal. Some parameters need attention.'
    case 'UNSAFE': 
      return 'Water quality is unsafe for drinking. Multiple parameters exceed safe limits.'
    default: 
      return 'Water quality status is unknown. Please check sensor connection.'
  }
}

// 根据水质评价标准检查个别参数状态
export const getParameterStatus = (param, value) => {
  switch(param) {
    case 'ph':
      // pH值标准：🟢 6.5-8.0, 🟡 6.0-6.4或8.1-9.0, 🔴 <6.0或>9.0
      if (value >= 6.5 && value <= 8.0) return 'excellent'
      if ((value >= 6.0 && value < 6.5) || (value > 8.0 && value <= 9.0)) return 'marginal'
      return 'unsafe'
      
    case 'turbidity':
      // 浊度标准：🟢 0-1.0 NTU, 🟡 1.1-4.0 NTU, 🔴 >4.0 NTU
      if (value >= 0 && value <= 1.0) return 'excellent'
      if (value > 1.0 && value <= 4.0) return 'marginal'
      return 'unsafe'
      
    case 'tds':
      // TDS标准：🟢 80-300 ppm, 🟡 300-500或50-79 ppm, 🔴 >500或<50 ppm
      if (value >= 80 && value <= 300) return 'excellent'
      if ((value >= 50 && value < 80) || (value > 300 && value <= 500)) return 'marginal'
      return 'unsafe'
      
    case 'conductivity':
      // 电导率标准：🟢 100-400 μS/cm, 🟡 400-800 μS/cm, 🔴 >800或<50 μS/cm
      if (value >= 100 && value <= 400) return 'excellent'
      if (value > 400 && value <= 800) return 'marginal'
      return 'unsafe'
      
    case 'temperature':
      // 温度参考范围（基本检查）
      if (value >= 5 && value <= 25) return 'excellent'
      if (value >= 0 && value <= 35) return 'marginal'
      return 'unsafe'
      
    default:
      return 'excellent'
  }
}

// 获取参数状态描述
export const getParameterDescription = (param, value, status) => {
  const descriptions = {
    excellent: {
      ph: 'Ideal drinking water pH range',
      turbidity: 'Crystal clear water - Excellent quality',
      tds: 'Ideal mineral content for health',
      conductivity: 'Good drinking water quality',
      temperature: 'Optimal temperature'
    },
    marginal: {
      ph: 'Acceptable but not ideal pH level',
      turbidity: 'Good clarity - Needs attention',
      tds: 'Acceptable mineral content',
      conductivity: 'Acceptable conductivity level',
      temperature: 'Acceptable temperature range'
    },
    unsafe: {
      ph: 'pH level unsafe for drinking',
      turbidity: 'High turbidity - Not suitable',
      tds: value < 50 ? 'Too low - Lacks minerals' : 'Too high - Excessive dissolved solids',
      conductivity: value < 50 ? 'Too pure - Lacks minerals' : 'Too high - Excessive salts',
      temperature: 'Temperature outside safe range'
    }
  }
  
  return descriptions[status]?.[param] || 'Status unknown'
}

// 获取参数参考范围文本
export const getParameterReference = (param) => {
  const references = {
    ph: '🟢 6.5-8.0 | 🟡 6.0-6.4, 8.1-9.0 | 🔴 <6.0, >9.0',
    turbidity: '🟢 0-1.0 | 🟡 1.1-4.0 | 🔴 >4.0 NTU',
    tds: '🟢 80-300 | 🟡 50-79, 300-500 | 🔴 <50, >500 ppm',
    conductivity: '🟢 100-400 | 🟡 400-800 | 🔴 <50, >800 μS/cm',
    temperature: '🟢 5-25 | 🟡 0-35 | 🔴 <0, >35 °C'
  }
  
  return references[param] || ''
}

// 数据源显示文本 - 更新为支持Neon数据库
export const getDataSourceText = (source) => {
  const sourceConfig = {
    // 主要数据源 - Neon数据库
    'neon_database': '💾 Neon Database (Real-time)',
    'neon_database_old': '💾 Neon Database (Older data)',
    
    // TTN相关数据源 (作为后备)
    'ttn_storage': '🌐 TTN Message Storage',
    'ttn_uplink': '📡 TTN Live Data Stream',
    'ttn_fallback': '📡 TTN Fallback Data',
    
    // 其他数据源
    'database': '💾 Local Database',
    'database_fallback': '💾 Database Backup',
    'mock': '🎭 Demo Data',
    
    // Webhook数据源
    'webhook': '🔗 Direct Webhook',
    'api_direct': '🚀 Direct API'
  }
  return sourceConfig[source] || source
}

// 格式化时间显示
export const formatTime = (currentTime, mounted) => {
  if (!mounted || !currentTime) return '--:--:--'
  return currentTime.toLocaleTimeString()
}

// 格式化日期显示
export const formatDate = (currentTime, mounted) => {
  if (!mounted || !currentTime) return 'Loading...'
  return currentTime.toLocaleDateString()
}

// 获取数据新鲜度状态
export const getDataFreshnessStatus = (lastUpdate) => {
  if (!lastUpdate) return { status: 'unknown', text: 'Unknown', color: '#6B7280' }
  
  const now = new Date()
  const updateTime = new Date(lastUpdate)
  const ageMinutes = (now - updateTime) / (1000 * 60)
  
  if (ageMinutes < 5) {
    return { status: 'fresh', text: 'Very Fresh', color: '#10B981' }
  } else if (ageMinutes < 15) {
    return { status: 'recent', text: 'Recent', color: '#F59E0B' }
  } else if (ageMinutes < 60) {
    return { status: 'old', text: 'Older', color: '#EF4444' }
  } else {
    return { status: 'stale', text: 'Stale', color: '#6B7280' }
  }
}

// 格式化数据年龄
export const formatDataAge = (dataAgeSeconds) => {
  if (!dataAgeSeconds || dataAgeSeconds < 0) return 'Unknown'
  
  if (dataAgeSeconds < 60) {
    return `${dataAgeSeconds}s ago`
  } else if (dataAgeSeconds < 3600) {
    return `${Math.round(dataAgeSeconds / 60)}m ago`
  } else if (dataAgeSeconds < 86400) {
    return `${Math.round(dataAgeSeconds / 3600)}h ago`
  } else {
    return `${Math.round(dataAgeSeconds / 86400)}d ago`
  }
}