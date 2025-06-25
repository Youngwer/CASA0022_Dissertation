// components/ErrorBanner.js - 错误提示横幅组件

const ErrorBanner = ({ error, onRetry }) => {
  if (!error) return null

  return (
    <div className="error-banner">
      <div className="error-content">
        <span className="error-icon">⚠️</span>
        <span className="error-message">{error}</span>
      </div>
      <button onClick={onRetry} className="retry-button">
        🔄 Retry
      </button>
    </div>
  )
}

export default ErrorBanner