// components/ContactModal.js - Contact Us弹窗组件

const ContactModal = ({ isOpen, onClose }) => {
  if (!isOpen) return null

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="contact-modal" onClick={(e) => e.stopPropagation()}>
        <div className="modal-header">
          <h3 className="contact-title">
            <span>📧</span>
            Contact Us
          </h3>
          <button className="close-button" onClick={onClose} title="Close">
            ×
          </button>
        </div>
        
        <div className="contact-content">
          <p className="contact-description">
            Get in touch with the AquaMonitor development team. We'd love to hear from you!
          </p>
          
          <div className="contact-links">
            <a
            className="contact-link email"
            onClick={(e) => e.preventDefault()}
            rel="noopener noreferrer"
            >
            <span className="contact-icon">📧</span>
            <span className="contact-text">ucfnwy2@ucl.ac.uk</span>
            </a>
            <a 
              href="https://github.com/Youngwer/CASA0022_Dissertation" 
              className="contact-link github"
              target="_blank"
              rel="noopener noreferrer"
            >
              <span className="contact-icon">🐱</span>
              <span className="contact-text">GitHub Repository</span>
            </a>
          </div>
          
          <p style={{ fontSize: '0.8rem', color: '#666', marginTop: '1rem' }}>
            Feel free to report issues, suggest improvements, or contribute to the project!
          </p>
        </div>
      </div>
    </div>
  )
}

export default ContactModal