// pages/index.js - 重构后的水质监测仪表板主页面

import Head from 'next/head'
import Header from '../components/Header'
import StatusOverview from '../components/StatusOverview'
import ParametersGrid from '../components/ParametersGrid'
import ErrorBanner from '../components/ErrorBanner'
import Footer from '../components/Footer'
import useWaterData from '../hooks/useWaterData'
import useTime from '../hooks/useTime'

export default function Home() {
  // 使用自定义Hooks
  const {
    waterData,
    dataSource,
    loading,
    error,
    isOnline,
    refreshData
  } = useWaterData()
  
  const {
    currentTime,
    mounted
  } = useTime()

  return (
    <div className="dashboard">
      <Head>
        <title>AquaMonitor - Water Quality Dashboard</title>
        <meta name="description" content="Real-time Water Quality Monitoring & Analysis System" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="icon" href="/favicon.ico" />
        <meta name="keywords" content="water quality, IoT, LoRaWAN, monitoring, Arduino, sensors" />
        <meta name="author" content="AquaMonitor Team" />
      </Head>

      {/* 头部导航（包含快捷操作） */}
      <Header 
        currentTime={currentTime}
        mounted={mounted}
        isOnline={isOnline}
        loading={loading}
        dataSource={dataSource}
        onRefresh={refreshData}
      />

      {/* 主要内容区域 */}
      <main className="main">
        <div className="container">
          
          {/* 错误提示横幅 */}
          <ErrorBanner 
            error={error}
            onRetry={refreshData}
          />
          
          {/* 水质状态总览（包含温度） */}
          <StatusOverview 
            waterData={waterData}
            dataSource={dataSource}
            onDataRefresh={refreshData}
          />

          {/* 参数卡片网格（只显示核心4个参数） */}
          <ParametersGrid 
            waterData={waterData}
          />

        </div>
      </main>

      {/* 页脚 */}
      <Footer />
    </div>
  )
}