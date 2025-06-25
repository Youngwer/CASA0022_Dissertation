// components/Footer.js - 页脚组件

const Footer = () => {
  return (
    <footer className="footer">
      <div className="container">
        <div className="footer-content">
          <div className="footer-info">
            <p>&copy; 2025 AquaMonitor - Making Every Drop Trustworthy</p>
            <p>Powered by IoT & LoRaWAN Technology • Real-time Sensor Data</p>
          </div>
          
          <div className="footer-links">
            <a href="#about" className="footer-link">About</a>
            <a href="#contact" className="footer-link">Contact</a>
            <a href="#privacy" className="footer-link">Privacy</a>
            <a href="#terms" className="footer-link">Terms</a>
          </div>
          
          <div className="footer-tech">
            <div className="tech-stack">
              <span className="tech-item">Arduino</span>
              <span className="tech-item">LoRaWAN</span>
              <span className="tech-item">TTN</span>
              <span className="tech-item">Next.js</span>
              <span className="tech-item">Vercel</span>
            </div>
          </div>
        </div>
      </div>
    </footer>
  )
}

export default Footer