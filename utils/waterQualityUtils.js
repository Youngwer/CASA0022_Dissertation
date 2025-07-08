// utils/waterQualityUtils.js - 改进的水质评估工具函数

// 获取水质状态总体描述
export const getWaterQualityDescription = (status) => {
  switch(status) {
    case 'EXCELLENT': 
      return 'All parameters are within ideal ranges.'
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

// 获取参数状态描述（显示在卡片中）
export const getParameterDescription = (param, value, status) => {
  const descriptions = {
    excellent: {
      ph: 'Ideal drinking water pH range',
      turbidity: 'Crystal clear water',
      tds: 'Ideal mineral content for health',
      conductivity: 'Good drinking water quality',
      temperature: 'Optimal temperature'
    },
    marginal: {
      ph: 'Acceptable but not ideal pH level',
      turbidity: 'Good clarity',
      tds: 'Acceptable mineral content',
      conductivity: 'Acceptable conductivity level',
      temperature: 'Acceptable temperature range'
    },
    unsafe: {
      ph: 'pH level unsafe for drinking',
      turbidity: 'High turbidity',
      tds: value < 50 ? 'Too low - Lacks minerals' : 'Too high - Excessive dissolved solids',
      conductivity: value < 50 ? 'Too pure - Lacks minerals' : 'Too high - Excessive salts',
      temperature: 'Temperature outside safe range'
    }
  }
  
  return descriptions[status]?.[param] || 'Status unknown'
}

// 获取参考范围（显示在卡片中，保留科普价值，分三行显示）
export const getParameterReference = (param) => {
  const references = {
    ph: '🟢 6.5-8.0\n🟡 6.0-6.4, 8.1-9.0\n🔴 <6.0, >9.0',
    turbidity: '🟢 0-1.0 NTU\n🟡 1.1-4.0 NTU\n🔴 >4.0 NTU',
    tds: '🟢 80-300 ppm\n🟡 50-79, 300-500 ppm\n🔴 <50, >500 ppm',
    conductivity: '🟢 100-400 μS/cm\n🟡 50-99, 400-800 μS/cm\n🔴 <50, >800 μS/cm',
    temperature: '🟢 5-25°C\n🟡 0-35°C\n🔴 <0, >35°C'
  }
  
  return references[param]
}

// 简化的英文悬浮提示内容（仅解释参数含义）
export const getParameterTooltip = (param) => {
  const tooltips = {
    ph: "pH measures the acidity or alkalinity of water, affecting taste and safety.",
    turbidity: "Turbidity indicates the cloudiness of water caused by suspended particles.",
    conductivity: "Conductivity reflects the total amount of ions in water, related to mineral content.",
    tds: "Total Dissolved Solids (TDS) represents the concentration of dissolved minerals in water.",
    temperature: "Temperature affects the taste and biological activity in water."
  }
  return tooltips[param] || "This parameter indicates water quality."
}

// 综合水质状态评估
export const evaluateOverallWaterQuality = (ph, turbidity, tds, conductivity) => {
  let issues = 0

  // pH检查
  if (ph < 6.0 || ph > 9.0) issues += 2
  else if (ph < 6.5 || ph > 8.0) issues += 1

  // 浊度检查
  if (turbidity > 4.0) issues += 2
  else if (turbidity > 1.0) issues += 1

  // TDS检查
  if (tds < 50 || tds > 500) issues += 2
  else if (tds < 80 || tds > 300) issues += 1

  // 电导率检查
  if (conductivity < 50 || conductivity > 800) issues += 2
  else if (conductivity < 100 || conductivity > 400) issues += 1

  if (issues >= 3) return 'UNSAFE'
  else if (issues >= 1) return 'MARGINAL'
  else return 'EXCELLENT'
}

// ==================== 时间和日期格式化函数 ==================== 
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

// ==================== 数据源和状态相关函数 ==================== 
// 数据源显示文本
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

// 获取状态颜色
export const getStatusColor = (status) => {
  switch(status) {
    case 'EXCELLENT': return 'rgba(16, 185, 129, 0.8)'
    case 'MARGINAL': return 'rgba(245, 158, 11, 0.8)'  
    case 'UNSAFE': return 'rgba(239, 68, 68, 0.8)'
    default: return 'rgba(107, 114, 128, 0.8)'
  }
}

// 获取状态文本
export const getStatusText = (status) => {
  switch(status) {
    case 'EXCELLENT': return 'Excellent'
    case 'MARGINAL': return 'Marginal'
    case 'UNSAFE': return 'Unsafe'
    default: return 'Unknown'
  }
}

// 获取状态描述
export const getStatusDescription = (status) => {
  return getWaterQualityDescription(status)
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